#include "soft_i2c.h"

// I2C 延时
static void I2C_Delay(int microseconds)
{
    DWT_Delay_us(microseconds);
}

// ==================== I2C 基本信号 ====================

void Soft_I2C_Start(Soft_I2C_t *soft_i2c, uint32_t delay_us)
{
    // SDA和SCL先拉高
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_SET);
    I2C_Delay(delay_us);
    
    // SDA拉低（产生起始信号）
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin, GPIO_PIN_RESET);
    I2C_Delay(delay_us);
    
    // SCL拉低（准备发送数据）
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_RESET);
    I2C_Delay(delay_us);
}

void Soft_I2C_Stop(Soft_I2C_t *soft_i2c, uint32_t delay_us)
{
    // SDA先拉低，SCL拉高
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_SET);
    I2C_Delay(delay_us);
    
    // SDA拉高（产生停止信号）
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin, GPIO_PIN_SET);
    I2C_Delay(delay_us);
}

// ==================== ACK/NACK ====================

void Soft_I2C_Ack(Soft_I2C_t *soft_i2c, uint32_t delay_us)
{
    // 主机发送ACK：拉低SDA
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin, GPIO_PIN_RESET);
    I2C_Delay(delay_us);
    
    // 产生第9个时钟
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_SET);
    I2C_Delay(delay_us);
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_RESET);
    I2C_Delay(delay_us);
    
    // 释放SDA
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin, GPIO_PIN_SET);
    I2C_Delay(delay_us);
}

void Soft_I2C_NAck(Soft_I2C_t *soft_i2c, uint32_t delay_us)
{
    // 主机发送NACK：保持SDA高
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin, GPIO_PIN_SET);
    I2C_Delay(delay_us);
    
    // 产生第9个时钟
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_SET);
    I2C_Delay(delay_us);
    
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_RESET);
    I2C_Delay(delay_us);
}

uint8_t Soft_I2C_Wait_Ack(Soft_I2C_t *soft_i2c, uint32_t delay_us)
{
    // 1. 释放SDA（让从机控制）
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin, GPIO_PIN_SET);
    DWT_Delay_us(delay_us);     // 等待从机响应
    
    // 2. 拉高SCL（第9个时钟）
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_SET);
    DWT_Delay_us(delay_us);     // 等待SDA稳定
    
    // 3. 读取SDA（检查从机是否拉低）
    uint8_t ack = HAL_GPIO_ReadPin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin);
    
    // 4. 拉低SCL
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_RESET);
    DWT_Delay_us(delay_us);
    
    // 返回ACK
    return ack;     // ACK = 0, NACK = 1
}

// ==================== 读写字节 ====================

uint8_t Soft_I2C_WriteByte(Soft_I2C_t *soft_i2c, uint8_t data, uint32_t delay_us)
{
    // 发送8位数据（从高位到低位）
    for(uint8_t i = 0; i < 8; i++) 
    {
        // 设置SDA（在SCL低电平时）
        HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        data <<= 1;
        DWT_Delay_us(delay_us);
        
        // 拉高SCL（从机采样）
        HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_SET);
        DWT_Delay_us(delay_us);
        
        // 拉低SCL
        HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_RESET);
        DWT_Delay_us(delay_us);
    }
    
    // 等待ACK
    return Soft_I2C_Wait_Ack(soft_i2c, delay_us);
}

uint8_t Soft_I2C_ReadByte(Soft_I2C_t *soft_i2c, uint8_t ack, uint32_t delay_us)
{
    uint8_t data = 0;
    
    // 释放SDA（让从机控制）
    HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin, GPIO_PIN_SET);
    I2C_Delay(delay_us);
    
    // 读取8位数据（从高位到低位）
    for(uint8_t i = 0; i < 8; i++) {
        data <<= 1;
        
        // 拉高SCL
        HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_SET);
        I2C_Delay(delay_us);  // 等待SDA稳定
        
        // 读取SDA
        if(HAL_GPIO_ReadPin(soft_i2c->GPIO_Port, soft_i2c->SDA_Pin) == GPIO_PIN_SET) {
            data |= 0x01;
        }
        
        // 拉低SCL
        HAL_GPIO_WritePin(soft_i2c->GPIO_Port, soft_i2c->SCL_Pin, GPIO_PIN_RESET);
        I2C_Delay(delay_us);
    }
    
    // 发送ACK或NACK
    if(ack) {
        Soft_I2C_Ack(soft_i2c, delay_us);    // ACK：继续读
    } else {
        Soft_I2C_NAck(soft_i2c, delay_us);   // NACK：停止读
    }

    return data;
}

// ==================== 寄存器读写封装 ==================== //

// 写寄存器：Start → Write(设备地址) → Write(寄存器) → Write(数据) → Stop
void Soft_I2C_WriteRegister(Soft_I2C_t *soft_i2c, uint8_t dev_addr, uint8_t reg_addr, uint8_t data, uint32_t delay_us)
{
    Soft_I2C_Start(soft_i2c, delay_us);
    Soft_I2C_WriteByte(soft_i2c, dev_addr << 1, delay_us);     // 写地址
    Soft_I2C_WriteByte(soft_i2c, reg_addr, delay_us);          // 寄存器地址
    Soft_I2C_WriteByte(soft_i2c, data, delay_us);             // 写入数据
    Soft_I2C_Stop(soft_i2c, delay_us);
}

// 读多个寄存器：Start → Write(设备地址) → Write(寄存器) → Repeated Start → Write(设备地址|读) → Read(N-1次ACK) → Read(最后1次NACK) → Stop
void Soft_I2C_ReadRegisters(Soft_I2C_t *soft_i2c, uint8_t dev_addr, uint8_t reg_addr, uint8_t *receive_buff, uint8_t size, uint32_t delay_us)
{
    Soft_I2C_Start(soft_i2c, delay_us);
    Soft_I2C_WriteByte(soft_i2c, dev_addr << 1, delay_us);         // 写地址
    Soft_I2C_WriteByte(soft_i2c, reg_addr, delay_us);              // 寄存器地址
    Soft_I2C_Start(soft_i2c, delay_us);                            // Repeated Start
    Soft_I2C_WriteByte(soft_i2c, (dev_addr << 1) | 0x01, delay_us); // 读地址

    for(uint8_t i = 0; i < size - 1; i++)
    {
        receive_buff[i] = Soft_I2C_ReadByte(soft_i2c, 1, delay_us);  // ACK：继续读
    }
    receive_buff[size - 1] = Soft_I2C_ReadByte(soft_i2c, 0, delay_us); // NACK：停止读
    Soft_I2C_Stop(soft_i2c, delay_us);
}
