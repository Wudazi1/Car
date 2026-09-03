#include "bh1721.h"

// BH1721 软件I2C对象（与OLED共用PD10/PD11）
Soft_I2C_t bh1721_i2c = {
    .GPIO_Port = GPIOD,
    .SDA_Pin   = GPIO_PIN_11,
    .SCL_Pin   = GPIO_PIN_10
};

// 初始化BH1721
void BH1721_Init(void)
{
    Soft_I2C_Start(&bh1721_i2c, BH1721_I2C_DELAY);
    if (Soft_I2C_WriteByte(&bh1721_i2c, BH1721_ADDR << 1, BH1721_I2C_DELAY))
    {
        printf("BH1721 address not ACK!\n");
    }
    Soft_I2C_WriteByte(&bh1721_i2c, 0x01, BH1721_I2C_DELAY); // 上电命令
    Soft_I2C_Stop(&bh1721_i2c, BH1721_I2C_DELAY);

    HAL_Delay(100);

    Soft_I2C_Start(&bh1721_i2c, BH1721_I2C_DELAY);
    if (Soft_I2C_WriteByte(&bh1721_i2c, BH1721_ADDR << 1, BH1721_I2C_DELAY))
    {
        printf("BH1721 address not ACK!\n");
    }
    Soft_I2C_WriteByte(&bh1721_i2c, 0x10, BH1721_I2C_DELAY); // 设置测量模式
    Soft_I2C_Stop(&bh1721_i2c, BH1721_I2C_DELAY);

    HAL_Delay(100);
}

// 读取BH1721的数据
uint16_t BH1721_ReadData(void)
{
    Soft_I2C_Start(&bh1721_i2c, BH1721_I2C_DELAY);
    if (Soft_I2C_WriteByte(&bh1721_i2c, (BH1721_ADDR << 1) | 0x01, BH1721_I2C_DELAY))
    {
        printf("BH1721 read address not ACK!\n");
        Soft_I2C_Stop(&bh1721_i2c, BH1721_I2C_DELAY);
        return 0xFFFF;
    }

    uint8_t msb = Soft_I2C_ReadByte(&bh1721_i2c, 1, BH1721_I2C_DELAY);    // 带ACK读取MSB (ack=1)
    uint8_t lsb = Soft_I2C_ReadByte(&bh1721_i2c, 0, BH1721_I2C_DELAY);    // 带NACK读取LSB (ack=0)
    Soft_I2C_Stop(&bh1721_i2c, BH1721_I2C_DELAY);

    return (msb << 8) | lsb;
}
