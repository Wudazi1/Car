#include "eeprom.h"

static Soft_I2C_t eeprom_i2c = {
    .GPIO_Port = GPIOE,
    .SDA_Pin   = GPIO_PIN_3,
    .SCL_Pin   = GPIO_PIN_4
};

// 向EEPROM写入单字节
void EEPROM_WriteByte(uint16_t addr, uint8_t data)
{
    Soft_I2C_Start(&eeprom_i2c, EEPROM_I2C_DELAY_US);
    Soft_I2C_WriteByte(&eeprom_i2c, EEPROM_I2C_ADDR << 1, EEPROM_I2C_DELAY_US);
    Soft_I2C_WriteByte(&eeprom_i2c, addr, EEPROM_I2C_DELAY_US);
    Soft_I2C_WriteByte(&eeprom_i2c, data, EEPROM_I2C_DELAY_US);
    Soft_I2C_Stop(&eeprom_i2c, EEPROM_I2C_DELAY_US);
    
    HAL_Delay(5);
}

// 从EEPROM读取单字节
uint8_t EEPROM_ReadByte(uint16_t addr)
{
    uint8_t data = 0;
    
    // 写地址
    Soft_I2C_Start(&eeprom_i2c, EEPROM_I2C_DELAY_US);
    Soft_I2C_WriteByte(&eeprom_i2c, EEPROM_I2C_ADDR << 1, EEPROM_I2C_DELAY_US);
    
    Soft_I2C_WriteByte(&eeprom_i2c, addr, EEPROM_I2C_DELAY_US);
    
    // 读地址
    Soft_I2C_Start(&eeprom_i2c, EEPROM_I2C_DELAY_US);
    Soft_I2C_WriteByte(&eeprom_i2c, (EEPROM_I2C_ADDR<<1)|0x01, EEPROM_I2C_DELAY_US);
    
    // 读取数据
    data = Soft_I2C_ReadByte(&eeprom_i2c, 1, EEPROM_I2C_DELAY_US);
    Soft_I2C_Stop(&eeprom_i2c, EEPROM_I2C_DELAY_US);
    
    return data;
}
