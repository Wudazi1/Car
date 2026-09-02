#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#include "system_bsp.h"

typedef struct {
    GPIO_TypeDef *GPIO_Port;
    uint16_t SDA_Pin;
    uint16_t SCL_Pin;
} Soft_I2C_t;

// 函数声明
void Soft_I2C_Start(Soft_I2C_t *soft_i2c, uint32_t delay_us);
void Soft_I2C_Stop(Soft_I2C_t *soft_i2c, uint32_t delay_us);
void Soft_I2C_Ack(Soft_I2C_t *soft_i2c, uint32_t delay_us);
void Soft_I2C_NAck(Soft_I2C_t *soft_i2c, uint32_t delay_us);
uint8_t Soft_I2C_Wait_Ack(Soft_I2C_t *soft_i2c, uint32_t delay_us);
uint8_t Soft_I2C_WriteByte(Soft_I2C_t *soft_i2c, uint8_t data, uint32_t delay_us);
uint8_t Soft_I2C_ReadByte(Soft_I2C_t *soft_i2c, uint8_t ack, uint32_t delay_us);

#endif
