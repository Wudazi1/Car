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

void EEPROM_SaveOdometer(float odometer)
{
	// 将float转换为4个字节存储
	uint8_t *bytes = (uint8_t *)&odometer;

	// 写入4个字节到EEPROM
	EEPROM_WriteByte(EEPROM_ODOMETER_ADDR, bytes[0]);
	EEPROM_WriteByte(EEPROM_ODOMETER_ADDR + 1, bytes[1]);
	EEPROM_WriteByte(EEPROM_ODOMETER_ADDR + 2, bytes[2]);
	EEPROM_WriteByte(EEPROM_ODOMETER_ADDR + 3, bytes[3]);
}

float EEPROM_LoadOdometer(void)
{
	float odometer = 0.0f;
	uint8_t *bytes = (uint8_t *)&odometer;

	// 从EEPROM读取4个字节
	bytes[0] = EEPROM_ReadByte(EEPROM_ODOMETER_ADDR);
	bytes[1] = EEPROM_ReadByte(EEPROM_ODOMETER_ADDR + 1);
	bytes[2] = EEPROM_ReadByte(EEPROM_ODOMETER_ADDR + 2);
	bytes[3] = EEPROM_ReadByte(EEPROM_ODOMETER_ADDR + 3);

	return odometer;
}

void eeprom_proc(void)
{
	static float last_odometer = 0.0f;
	float current_odometer = get_robot_odometer();

	if(fabs(current_odometer - last_odometer) > 0.1f)
	{
		EEPROM_SaveOdometer(current_odometer);
		last_odometer = current_odometer;
	}
}
