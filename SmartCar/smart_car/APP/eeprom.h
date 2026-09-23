#ifndef __EEPROM_H
#define __EEPROM_H

#include "system_bsp.h"

// EEPROM配置
#define EEPROM_I2C_ADDR         0x50  
#define EEPROM_I2C_DELAY_US       5  

void EEPROM_WriteByte(uint16_t addr, uint8_t data);
uint8_t EEPROM_ReadByte(uint16_t addr);
void EEPROM_SaveOdometer(float odometer);
float EEPROM_LoadOdometer(void);
void eeprom_proc(void);

#define EEPROM_ODOMETER_ADDR    0x00    // 里程存储首地址


#endif
