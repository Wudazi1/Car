#include "ps2.h"
#include <string.h>
#include "stdio.h"

// 外部SPI句柄声明
extern SPI_HandleTypeDef hspi2;

// 全局变量
unsigned int Handkey;   // 按键值读取，临时存储
uint8_t ps2_mode;       // 手柄模式

// 发送命令数组
uint8_t Comd[9] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// 接收数据数组
uint8_t Data[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// 按键状态映射表
unsigned int MASK[16][2] = {
    {PSB_SELECT, 0},
    {PSB_L3, 0},
    {PSB_R3, 0},
    {PSB_START, 0},
    {PSB_PAD_UP, 0},
    {PSB_PAD_RIGHT, 0},
    {PSB_PAD_DOWN, 0},
    {PSB_PAD_LEFT, 0},
    {PSB_L2, 0},
    {PSB_R2, 0},
    {PSB_L1, 0},
    {PSB_R1, 0},
    {PSB_GREEN, 0},
    {PSB_RED, 0},
    {PSB_BLUE, 0},
    {PSB_PINK, 0}
};

// 硬件SPI底层收发函数(不控制CS)
static uint8_t PS2_SPI_TransmitReceive(uint8_t TxData)
{
    uint8_t RxData = 0;

    if(HAL_SPI_TransmitReceive(&hspi2, &TxData, &RxData, 1, 1000) != HAL_OK)
    {
        printf("SPI TransmitReceive Error!\r\n");
    }

    return RxData;
}

// 硬件SPI收发函数(封装CS控制)
uint8_t PS2_SPI_ReadWriteByte(uint8_t TxData)
{
    uint8_t RxData = 0;

    PS2_CS_L();
    DWT_Delay_us(10);

    RxData = PS2_SPI_TransmitReceive(TxData);

    DWT_Delay_us(10);
    PS2_CS_H();

    return RxData;
}

// PS2初始化
void PS2_Init(void)
{       
    printf("Starting PS2 Hardware SPI Initialization...\r\n");
    
    // 1. CS初始化为高电平
    PS2_CS_H();
    
    // 2. 等待手柄准备好
    HAL_Delay(100);
    
    // 3. 三次短轮询建立通信
    printf("  Step 1: Short Poll 1...\r\n");
    PS2_ShortPoll();
    HAL_Delay(10);
    
    printf("  Step 2: Short Poll 2...\r\n");
    PS2_ShortPoll();
    HAL_Delay(10);
    
    printf("  Step 3: Short Poll 3...\r\n");
    PS2_ShortPoll();
    HAL_Delay(10);
    
    // 4. 读取一次数据检查连接
    printf("  Step 4: Reading initial data...\r\n");
    PS2_ReadData();

    // 5. 检查手柄模式
    if(Data[1] == 0x73 || Data[1] == 0x41)
    {
        printf("PS2 Hardware SPI Initialization Success!\r\n");
    }
    else
    {
        printf("PS2 Hardware SPI Initialization Failed!\r\n");
    }
}

// 读取手柄数据
void PS2_ReadData(void)
{
    uint8_t byte = 0;
    uint8_t rx_buf[9];

    // 准备发送数据
    PS2_CS_L();
    DWT_Delay_us(10);

    // 逐字节发送命令并接收数据
    for(byte = 0; byte < 9; byte++)
    {
        if(byte < 2)
        {
            rx_buf[byte] = PS2_SPI_TransmitReceive(Comd[byte]);
        }
        else
        {
            rx_buf[byte] = PS2_SPI_TransmitReceive(0x00);
        }
    }

    PS2_CS_H();
    DWT_Delay_us(10);

    // 复制接收数据
    for(byte = 0; byte < 9; byte++)
    {
        Data[byte] = rx_buf[byte];
    }
}

// 判断手柄模式
unsigned char ps2_mode_get(void)
{   
    if(Data[1] == 0x73)  
    {
        ps2_mode = PSB_ANALOG_MODE;
    }
    else if (Data[1] == 0x41)
    {
        ps2_mode = PSB_DIGITAL_MODE;
    }
    else
    {
        ps2_mode = PSB_LOSE;
    }
    return ps2_mode;
}

// 按键扫描处理
// 返回第一个被按下的按键ID（用于switch），如果没有按键按下返回0
unsigned char ps2_key_serch(void)
{
    unsigned char index;
    unsigned char first_key = 0;    // 第一个按下的键

    PS2_ReadData();
    Handkey = (Data[4] << 8) | Data[3];

    /* 第一步：完整更新所有键的状态 */
    for(index = 0; index < 16; index++)
    {
        if((Handkey & (1 << (MASK[index][0] - 1))) == 0)
        {
            MASK[index][1] = 1;             // 按下
            if(first_key == 0)              // 记录第一个按下的
                first_key = MASK[index][0];
        }
        else
        {
            MASK[index][1] = 0;             // 松开
        }
    }
    
    return first_key;    // 返回第一个按下的键 ID（0 = 无按键）
}

// 获取指定按键状态
unsigned char ps2_get_key_state(unsigned char key_id)
{
    if (key_id < PSB_SELECT || key_id > PSB_PINK)
        return 0;
    else
        return MASK[key_id - 1][1];
}

// 获取摇杆模拟量(0~255)
unsigned char ps2_get_anolog_data(unsigned char button)
{
    if (button >= 5 && button <= 8)
        return Data[button];
    else
        return 0;
}

// 清除数据缓冲区
void PS2_ClearData(void)
{
    unsigned char a;
    for(a = 0; a < 9; a++)
        Data[a] = 0x00;
}

// 短轮询函数
void PS2_ShortPoll(void)
{
    uint8_t byte = 0;

    PS2_CS_L();
    DWT_Delay_us(100);

    // 发送5字节短轮询命令
    for(byte = 0; byte < 5; byte++)
    {
        uint8_t tx_byte = 0x00;
        if(byte == 0) tx_byte = 0x01;
        else if(byte == 1) tx_byte = 0x42;

        PS2_SPI_TransmitReceive(tx_byte);
    }

    PS2_CS_H();
    DWT_Delay_us(100);
}

// 进入配置模式
void PS2_EnterConfing(void)
{
    uint8_t tx_buf[9] = {0x01, 0x43, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t byte = 0;

    PS2_CS_L();
    DWT_Delay_us(100);

    for(byte = 0; byte < 9; byte++)
    {
        PS2_SPI_TransmitReceive(tx_buf[byte]);
    }

    PS2_CS_H();
    DWT_Delay_us(100);
}

// 设置模拟模式
void PS2_TurnOnAnalogMode(void)
{
    uint8_t tx_buf[9] = {0x01, 0x44, 0x00, 0x01, 0xEE, 0x00, 0x00, 0x00, 0x00};
    uint8_t byte = 0;

    PS2_CS_L();
    DWT_Delay_us(100);

    for(byte = 0; byte < 9; byte++)
    {
        PS2_SPI_TransmitReceive(tx_buf[byte]);
    }

    PS2_CS_H();
    DWT_Delay_us(100);
}

// 退出配置模式
void PS2_ExitConfing(void)
{
    uint8_t tx_buf[9] = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
    uint8_t byte = 0;

    PS2_CS_L();
    DWT_Delay_us(100);

    for(byte = 0; byte < 9; byte++)
    {
        PS2_SPI_TransmitReceive(tx_buf[byte]);
    }

    PS2_CS_H();
    DWT_Delay_us(100);
}

void ps2_remote_control(void)
{
        // 获取摇杆数据
        uint8_t ly = Data[PSS_LY]; // 用于前进后退 0到255 中间值为127
        uint8_t rx = Data[PSS_RX]; // 用于舵机转向 0到255 中间值为128

        // 电机控制：ly直接映射为速度m/s
        // ly范围0-255，127是中间值停止
        float target_speed_val = 0.0f;

        if(ly < 127) {
            // 前进：ly从127→0对应0→1.2 m/s
            target_speed_val = (127 - ly) / 127.0f * 1.2f;
        } else if(ly > 127) {
            // 后退：ly从127→255对应0→-1.2 m/s
            target_speed_val = (127 - ly) / 127.0f * 1.2f;
        } else {
            target_speed_val = 0.0f;
        }

        // 使用car_set_speed设置速度
        car_set_speed(target_speed_val);

        // ==================舵机控制================== // 
        uint8_t front_angle = 90, tall_angle = 90; // 默认居中

        // 中间死区
        if(rx > 120 && rx < 135)
        {
            front_angle = 90;
            tall_angle = 90;
        }
        // 左转控制 (rx <= 120)
        else if(rx <= 120)
        {
            // 计算front_angle: 0-90度
            // rx从120到0，front_angle从90到0
            uint16_t front_angle_calc = 0;

            if(rx < 20)  // 最小左转
            {
                front_angle_calc = 0;  // 0度
            }
            else  // rx >= 20
            {
                // 线性映射: rx从120到20，front_angle从90到0
                // 公式: front_angle = 0 + (rx-20) * 90 / 100
                front_angle_calc = (rx - 20) * 90 / 100;
            }

            front_angle = front_angle_calc;       // 90-0度
            tall_angle = 180 - front_angle_calc;  // 90-180度

            // 限幅
            if(front_angle > 90) front_angle = 90;
            if(tall_angle < 90) tall_angle = 90;
        }
        // 右转控制 (rx >= 135)
        else
        {
            // 计算front_angle: 90-180度
            // rx从135到255，front_angle从90到180
            uint16_t front_angle_calc = 0;

            if(rx > 235)  // 最大右转
            {
                front_angle_calc = 180;  // 180度
            }
            else  // rx <= 235
            {
                // 线性映射: rx从135到235，front_angle从90到180
                // 公式: front_angle = 90 + (rx-135) * 90 / 100
                front_angle_calc = 90 + (rx - 135) * 90 / 100;
            }

            front_angle = front_angle_calc;  // 90-180度
            tall_angle = 180 - front_angle_calc;  // 90-0度

            // 限幅
            if(front_angle < 90) front_angle = 90;
            if(front_angle > 180) front_angle = 180;
            if(tall_angle > 90) tall_angle = 90;
        }

        servo_set(0, front_angle);  // 前舵机 dir=0
        servo_set(1, tall_angle);    // 后舵机 dir=1
}

// ps2任务函数
void ps2_proc(void)
{
    static uint8_t last_key = 0;
    // 读取手柄数据
    uint8_t key_id = ps2_key_serch();
	
    // 获取模式
    ps2_mode_get();
    
    if(ps2_mode == PSB_ANALOG_MODE) // 模拟模式
    {
        ps2_remote_control();
	}
    
    if(key_id !=0 && key_id != last_key)
    {
        switch(key_id)
        {
            case PSB_PAD_UP:
                break;
            case PSB_PAD_DOWN:
                break;
            case PSB_PAD_LEFT:
                break;
            case PSB_PAD_RIGHT:
                break;
            default:
                break;
        }        
    }
    last_key = key_id;
}
