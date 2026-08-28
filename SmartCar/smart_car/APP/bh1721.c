#include "bh1721.h"

static void I2C_Delay(void)
{
    DWT_Delay_us(5);
}

void SoftI2C_Start(void)
{
    HAL_GPIO_WritePin(I2C_PORT, SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, SDA_PIN, GPIO_PIN_RESET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, GPIO_PIN_RESET);
}

void SoftI2C_Stop(void)
{
    HAL_GPIO_WritePin(I2C_PORT, SDA_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, SDA_PIN, GPIO_PIN_SET);
    I2C_Delay();
}

// 发送一个字节
uint8_t SoftI2C_WriteByte(uint8_t data) 
{
	for(uint8_t i = 0; i < 8; i++) {
		HAL_GPIO_WritePin(I2C_PORT, SDA_PIN, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		data <<= 1;
		HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, GPIO_PIN_SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, GPIO_PIN_RESET);
	}
	
	// 接收ACK
	HAL_GPIO_WritePin(I2C_PORT, SDA_PIN, GPIO_PIN_SET); // 释放SDA
	HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, GPIO_PIN_SET);
	uint8_t ack = HAL_GPIO_ReadPin(I2C_PORT, SDA_PIN);  // 读取ACK位
	HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, GPIO_PIN_RESET);
	return ack; // 返回0表示ACK成功
}

// 读取一个字节
uint8_t SoftI2C_ReadByte(uint8_t ack) 
{
	uint8_t data = 0;
	HAL_GPIO_WritePin(I2C_PORT, SDA_PIN, GPIO_PIN_SET); // 释放SDA
	
	for(uint8_t i = 0; i < 8; i++) 
	{
		HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, GPIO_PIN_SET);
		data <<= 1;
		if(HAL_GPIO_ReadPin(I2C_PORT, SDA_PIN)) data |= 0x01;
		HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, GPIO_PIN_RESET);
	}
	
	// 发送ACK/NACK
	HAL_GPIO_WritePin(I2C_PORT, SDA_PIN, ack ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(I2C_PORT, SDA_PIN, GPIO_PIN_SET); // 释放SDA
	
	return data;
}

// 初始化BH1721
void BH1721_Init(void) 
{  
	SoftI2C_Start();
	if(SoftI2C_WriteByte(BH1721_ADDR << 1)) 
	{ 
		printf("BH1721 address not ACK!\n");
	}
	SoftI2C_WriteByte(0x01); // 上电命令
	SoftI2C_Stop();
	
	HAL_Delay(100);
	
	SoftI2C_Start();
	if(SoftI2C_WriteByte(BH1721_ADDR << 1))
	{
		printf("BH1721 address not ACK!\n");
	}
	SoftI2C_WriteByte(0x10); // 设置测量模式
	SoftI2C_Stop();
	
	HAL_Delay(100);
}

// 读取BH1721的数据
uint16_t BH1721_ReadData(void) 
{
	SoftI2C_Start();
	if(SoftI2C_WriteByte((BH1721_ADDR << 1) | 0x01)) 
	{ 
		printf("BH1721 read address not ACK!\n");
		SoftI2C_Stop();
		return 0xFFFF;
	}
	
	uint8_t msb = SoftI2C_ReadByte(0);    // 带ACK读取MSB
	uint8_t lsb = SoftI2C_ReadByte(1);    // 带NACK读取LSB
	SoftI2C_Stop();
	
	return (msb << 8) | lsb;
}
