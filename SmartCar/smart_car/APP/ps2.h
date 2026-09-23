#ifndef __PS2_H
#define __PS2_H

#include "system_bsp.h"

// 引脚定义
#define PS2_CS_PORT        GPIOB
#define PS2_CS_PIN         GPIO_PIN_12

// 宏定义
#define PS2_CS_L()         HAL_GPIO_WritePin(PS2_CS_PORT, PS2_CS_PIN, GPIO_PIN_RESET)
#define PS2_CS_H()         HAL_GPIO_WritePin(PS2_CS_PORT, PS2_CS_PIN, GPIO_PIN_SET)
#define PS2_Delay_US(us)   HAL_Delay_us(us)  // 需要实现微秒延时

// PS2手柄模式定义
#define PSB_LOSE                0       // 连接丢失
#define PSB_ANALOG_MODE         1       // 绿灯模式（模拟摇杆模式）
#define PSB_DIGITAL_MODE        2       // 红灯模式（数字模式）


// PS2手柄按键ID定义
#define PSB_SELECT      1       // SELECT键
#define PSB_L3          2       // 左摇杆按下
#define PSB_R3          3       // 右摇杆按下
#define PSB_START       4       // START键
#define PSB_PAD_UP      5       // 方向上键
#define PSB_PAD_RIGHT   6       // 方向右键
#define PSB_PAD_DOWN    7       // 方向下键
#define PSB_PAD_LEFT    8       // 方向左键
#define PSB_L2          9       // L2键
#define PSB_R2          10      // R2键
#define PSB_L1          11      // L1键
#define PSB_R1          12      // R1键
#define PSB_GREEN       13      // △键（绿色）
#define PSB_RED         14      // ○键（红色）
#define PSB_BLUE        15      // ×键（蓝色）
#define PSB_PINK        16      // □键（粉色）

// 兼容性定义
#define PSB_TRIANGLE    13      // △键
#define PSB_CIRCLE      14      // ○键
#define PSB_CROSS       15      // ×键
#define PSB_SQUARE      16      // □键

// 摇杆数据索引定义
#define PSS_RX          5       // 右摇杆X轴                
#define PSS_RY          6       // 右摇杆Y轴
#define PSS_LX          7       // 左摇杆X轴
#define PSS_LY          8       // 左摇杆Y轴

// 摇杆中位值定义
#define PSS_RX_MID      0x7F    // 右摇杆X轴中位值
#define PSS_RY_MID      0x80    // 右摇杆Y轴中位值

// 外部变量声明
extern unsigned int Handkey;        // 手柄按键状态临时存储
extern uint8_t ps2_mode;            // 手柄当前模式
extern uint8_t Data[9];             // 手柄数据存储数组
extern unsigned int MASK[16][2];    // 按键映射表

// 函数声明
void DWT_Init(void);                                     // 初始化DWT计数器
void PS2_Init(void);                                     // PS2手柄初始化
void PS2_ReadData(void);                                 // 读取手柄数据
unsigned char ps2_mode_get(void);                        // 获取手柄模式
unsigned char ps2_key_serch(void);                       // 扫描按键状态
unsigned char ps2_get_key_state(unsigned char key_id);   // 获取指定按键状态
unsigned char ps2_get_anolog_data(unsigned char button); // 获取摇杆模拟量
void PS2_ClearData(void);                                // 清除数据缓冲区
void PS2_ShortPoll(void);                                // 短轮询命令
void PS2_EnterConfing(void);                             // 进入配置模式
void PS2_TurnOnAnalogMode(void);                         // 开启模拟模式
void PS2_ExitConfing(void);                              // 退出配置模式
void PS2_Delay_US(uint32_t us);                          // 微秒延时函数
uint8_t PS2_SPI_ReadWriteByte(uint8_t TxData);           // SPI收发单个字节
void ps2_proc(void);                                     // PS2任务函数

#endif
