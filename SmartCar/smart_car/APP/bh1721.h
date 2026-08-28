#ifndef __BH1721_H
#define __BH1721_H

#include "system_bsp.h"

#define BH1721_ADDR     0x23    // 7位设备地址

// 软件I2C引脚定义
#define I2C_PORT GPIOD
#define SCL_PIN GPIO_PIN_10
#define SDA_PIN GPIO_PIN_11

// 函数声明
void SoftI2C_Start(void);
void SoftI2C_Stop(void);
uint8_t SoftI2C_WriteByte(uint8_t data);
uint8_t SoftI2C_ReadByte(uint8_t ack);
void BH1721_Init(void);
uint16_t BH1721_ReadData(void);

#endif
