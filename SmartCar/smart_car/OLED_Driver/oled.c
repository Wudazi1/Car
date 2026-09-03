#include "oled.h"
#include "oledfont.h"

#define OLED_ADDRESS   0x78    // OLED I2C地址
#define OLED_I2C_DELAY 5       // 微秒延时

// 软件I2C对象（与BH1721共用PD10/PD11）
static Soft_I2C_t oled_i2c = {
    .GPIO_Port = GPIOD,
    .SDA_Pin   = GPIO_PIN_11,
    .SCL_Pin   = GPIO_PIN_10
};

// 向OLED发送命令
void OLED_WR_Command(uint8_t cmd)
{
    Soft_I2C_Start(&oled_i2c, OLED_I2C_DELAY);
    Soft_I2C_WriteByte(&oled_i2c, OLED_ADDRESS, OLED_I2C_DELAY);  // 从机地址（写）
    Soft_I2C_WriteByte(&oled_i2c, 0x00, OLED_I2C_DELAY);          // 写命令
    Soft_I2C_WriteByte(&oled_i2c, cmd, OLED_I2C_DELAY);
    Soft_I2C_Stop(&oled_i2c, OLED_I2C_DELAY);
}

// 向OLED发送数据
void OLED_WR_Data(uint8_t dat)
{
    Soft_I2C_Start(&oled_i2c, OLED_I2C_DELAY);
    Soft_I2C_WriteByte(&oled_i2c, OLED_ADDRESS, OLED_I2C_DELAY);  // 从机地址（写）
    Soft_I2C_WriteByte(&oled_i2c, 0x40, OLED_I2C_DELAY);          // 写数据
    Soft_I2C_WriteByte(&oled_i2c, dat, OLED_I2C_DELAY);
    Soft_I2C_Stop(&oled_i2c, OLED_I2C_DELAY);
}

// OLED初始化
void OLED_Init(void)
{
    // OLED初始化序列
    HAL_Delay(100); // 原来为200ms
    OLED_WR_Command(0xAE); // 关闭显示
    OLED_WR_Command(0x20); // 设置内存寻址模式
    OLED_WR_Command(0x10); // 水平寻址模式
    OLED_WR_Command(0xb0); // 设置页地址
    OLED_WR_Command(0xc8); // 设置COM扫描方向
    OLED_WR_Command(0x00); // 设置列地址低4位
    OLED_WR_Command(0x10); // 设置列地址高4位
    OLED_WR_Command(0x40); // 设置显示开始行
    OLED_WR_Command(0x81); // 设置对比度
    OLED_WR_Command(0xff);
    OLED_WR_Command(0xa1); // 设置段重映射
    OLED_WR_Command(0xa6); // 设置正常显示
    OLED_WR_Command(0xa8); // 设置多路复用率
    OLED_WR_Command(0x1f);
    OLED_WR_Command(0xd3); // 设置显示偏移
    OLED_WR_Command(0x00);
    OLED_WR_Command(0xd5); // 设置时钟分频因子
    OLED_WR_Command(0xf0);
    OLED_WR_Command(0xd9); // 设置预充电周期
    OLED_WR_Command(0x22);
    OLED_WR_Command(0xda); // 设置COM引脚硬件配置
    OLED_WR_Command(0x02);
    OLED_WR_Command(0xdb); // 设置VCOMH电压倍率
    OLED_WR_Command(0x49);
    OLED_WR_Command(0x8d); // 启用电荷泵
    OLED_WR_Command(0x14);
    OLED_WR_Command(0xaf); // 开启显示
}

// 设置 OLED 光标位置
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
    OLED_WR_Command(0xb0 + y);
    OLED_WR_Command(((x & 0xf0) >> 4) | 0x10);
    OLED_WR_Command(x & 0x0f);
}

// 清屏函数
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 4; i++)
    {
        OLED_WR_Command(0xB0 + i);
        OLED_WR_Command(0x00);
        OLED_WR_Command(0x10);
        for (n = 0; n < 128; n++)
        {
            OLED_WR_Data(0x00);
        }
    }
}

// 清除指定页（0~3）
void OLED_ClearPage(uint8_t page)
{
    uint8_t col;
    OLED_Set_Pos(0, page);          // 光标定位到该页的起始列（列0）
    for (col = 0; col < 128; col++) // 循环128次，写入所有列
    {
        OLED_WR_Data(0x00);           // 写入0，熄灭该页所有像素
    }
}

// 字符显示函数
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
    unsigned char c = chr - ' ';
    uint8_t i;

    // 所有字符显示后都清空最后一列，防止残留
    OLED_Set_Pos(x, y);
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Data(F8X16[c * 16 + i]);
    }

    OLED_Set_Pos(x, y + 1);
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Data(F8X16[c * 16 + i + 8]);
    }

    // 在字符后添加一列空白，确保无残留
    OLED_Set_Pos(x + 8, y);
    OLED_WR_Data(0x00);
    OLED_Set_Pos(x + 8, y + 1);
    OLED_WR_Data(0x00);
}

// 字符串显示函数
void OLED_ShowString(uint8_t x, uint8_t y, char *str)
{
    uint8_t i = 0;
    while (str[i] != '\0')
    {
        OLED_ShowChar(x, y, str[i]);
        x += 9;  // 8像素字符宽度 + 1像素间隔
        if(x > 120) break;
        i++;
    }
}

// 水平滚动函数
void OLED_ShowRowString(uint8_t x, uint8_t y, char *str)
{
    while(x>0)
    {
        OLED_ShowString(x,y,str);
        HAL_Delay(10);
        x--;
    }
}

// 显示十进制数字
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
    uint8_t t, temp;
    for (t = 0; t < len; t++)
    {
        temp = (num / (uint32_t)pow(10, len - t - 1)) % 10;
        OLED_ShowChar(x + t * 8, y, temp + '0');
    }
}

// 显示十六进制数字
void OLED_ShowHexNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
    uint8_t t, temp;
    for (t = 0; t < len; t++)
    {
        temp = (num >> ((len - t - 1) * 4)) & 0x0f;
        if (temp < 10) temp += '0';
        else temp += 'A' - 10;
        OLED_ShowChar(x + t * 8, y, temp);
    }
}

// 显示图案
void OLED_ShowPicture(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t *pic)
{
    uint8_t i, j;
    for (i = 0; i < height / 8; i++)
    {
        OLED_Set_Pos(x, y + i);
        for (j = 0; j < width; j++)
        {
            OLED_WR_Data(pic[i * width + j]);
        }
    }
}

// 显示汉字
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t no)
{
    uint8_t t, adder = 0;
    OLED_Set_Pos(x, y);
    for (t = 0; t < 16; t++)
    {
        OLED_WR_Data(Hzk[no][t]);
        adder += 1;
    }
    OLED_Set_Pos(x, y + 1);
    for (t = 0; t < 16; t++)
    {
        OLED_WR_Data(Hzk[no][t + 16]);
        adder += 1;
    }
}


/* ---------- 内部辅助函数（轻量级整数转换） ---------- */
static void reverse_str(char *str, int len)
{
    int i = 0, j = len - 1;
    while(i < j)
    {
        char c = str[i];
        str[i] = str[j];
        str[j] = c;
        i++; j--;
    }
}

static char *utoa(char *buf, unsigned int val, unsigned int base, int uppercase)
{
    const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    char *p = buf;

    if (val == 0)
    {
        *p++ = '0';
        *p = '\0';
        return buf;   // 返回起始地址
    }
    while (val > 0)
    {
        *p++ = digits[val % base];
        val /= base;
    }
    *p = '\0';
    reverse_str(buf, p - buf);

    return buf;       // 返回起始地址
}

static char *itoa(char *buf, int val, int base)
{
    unsigned int uval;
    char *p = buf;

    if (val < 0 && base == 10)
    {
        *p++ = '-';
        uval = -val;
    }
    else
    {
        uval = val;
    }
    utoa(p, uval, base, 0);   // 填充剩余部分

    return buf;               // 返回起始地址
}

/* ---------- 轻量级 vsprintf 实现（支持常用格式） ---------- */
static int mini_vsprintf(char *buf, const char *fmt, va_list args)
{
    char *out = buf;
    char temp[32];

    while (*fmt)
    {
        if (*fmt != '%')
        {
            *out++ = *fmt++;
            continue;
        }
        fmt++; // 跳过 '%'

        if (*fmt == '%') // "%%" 输出一个 '%'
        {
            *out++ = '%';
            fmt++;
            continue;
        }

        char spec = *fmt++;
        switch (spec)
        {
            case 'd':
            case 'i':
            {
                int v = va_arg(args, int);
                char *p = itoa(temp, v, 10);
                while (*p) *out++ = *p++;
                break;
            }
            case 'u':
            {
                unsigned int v = va_arg(args, unsigned int);
                char *p = utoa(temp, v, 10, 0);
                while (*p) *out++ = *p++;
                break;
            }
            case 'x':
            {
                unsigned int v = va_arg(args, unsigned int);
                char *p = utoa(temp, v, 16, 0);
                while (*p) *out++ = *p++;
                break;
            }
            case 'X':
            {
                unsigned int v = va_arg(args, unsigned int);
                char *p = utoa(temp, v, 16, 1);
                while (*p) *out++ = *p++;
                break;
            }
            case 'c':
            {
                char ch = (char)va_arg(args, int);
                *out++ = ch;
                break;
            }
            case 's':
            {
                const char *s = va_arg(args, const char *);
                if (!s) s = "(null)";
                while (*s) *out++ = *s++;
                break;
            }
            default: // 不支持的格式，原样输出 % 和字符
                *out++ = '%';
                *out++ = spec;
                break;
        }
    }
    *out = '\0';
    return out - buf;
}

/* ---------- 对外 API：OLED 格式化显示 ---------- */
/**
 * @brief 在 OLED 指定位置显示格式化字符串（轻量级，不使用 sprintf）
 * @param x   起始列（0~127）
 * @param y   页（0~3）
 * @param fmt 格式字符串，支持：%d, %i, %u, %x, %X, %c, %s, %%
 * @param ... 可变参数
 * @note 内部缓冲区为64字节，确保格式化结果不超过一行显示长度（最多16个字符）
 */
void OLED_Printf(uint8_t x, uint8_t y, const char *fmt, ...)
{
    char buffer[64];
    va_list args;
    va_start(args, fmt);
    mini_vsprintf(buffer, fmt, args);
    va_end(args);
    OLED_ShowString(x, y, buffer);
}

// 表情显示函数
void OLED_DrawBitmap_Raw(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap)
{
    uint8_t page_start = y / 8;
    uint8_t page_end   = (y + h - 1) / 8;

    for (uint8_t page = page_start; page <= page_end; page++) {
        uint8_t page_y = page * 8;
        uint8_t offset = (page_y < y) ? (y - page_y) : 0;
        uint8_t rows   = (page_y + 8) > (y + h) ? (y + h - page_y) : 8;

        OLED_Set_Pos(x, page);

        for (uint8_t col = 0; col < w; col++) {
            uint8_t data = 0;
            for (uint8_t row_in_page = 0; row_in_page < rows; row_in_page++) {
                uint8_t abs_row = page_y + offset + row_in_page;
                if (abs_row >= y && abs_row < y + h) {
                    uint32_t bit_index = (abs_row - y) * w + col;
                    uint8_t byte_idx = bit_index / 8;
                    uint8_t bit_pos = 7 - (bit_index % 8);
                    if (bitmap[byte_idx] & (1 << bit_pos))
                        data |= (1 << row_in_page);
                }
            }
            OLED_WR_Data(data);
        }
    }
}
