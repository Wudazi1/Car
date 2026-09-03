#ifndef __BH1721_H
#define __BH1721_H

#include "system_bsp.h"

#define BH1721_ADDR      0x23    // 7位设备地址
#define BH1721_I2C_DELAY 5       // 微秒延时（与OLED共用同一I2C总线）

// BH1721 软件I2C对象（与OLED共用PD10/PD11）
extern Soft_I2C_t bh1721_i2c;

// 函数声明
void BH1721_Init(void);
uint16_t BH1721_ReadData(void);

#endif
