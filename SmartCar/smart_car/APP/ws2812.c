#include "ws2812.h"

extern DMA_HandleTypeDef hdma_tim1_ch1;
extern DMA_HandleTypeDef hdma_tim1_ch4_trig_com;

// 定义LED数量
#define LED_COUNT 28                   /* LED灯珠数量 */
#define DATA_SIZE (LED_COUNT * 3 * 8)  /* PWM数据大小：LED数量×3颜色×8位/颜色 */
/* DUTY（占空比）= CCR/(ARR+1) ，且ARR+1 = 90，0码->1/3的占空比，1码->2/3的占空比*/ 
#define HIGH_PULSE 60                  /* 高电平脉冲宽度（对应数据位1）*/
#define LOW_PULSE 30                   /* 低电平脉冲宽度（对应数据位0）*/
/* 低电平时间需大于125us，1.25us x 300远大于125us*/
#define WS2812_RST_NUM 300             /* WS2812复位脉冲数量，低电平时间需大于125us，1.25us x 300远大于125us*/

// 定义PWM数据数组
uint16_t pwm_data_frontled[DATA_SIZE + WS2812_RST_NUM];  /* 前灯PWM数据缓冲区 */
uint16_t pwm_data_rearled[DATA_SIZE + WS2812_RST_NUM];   /* 后灯PWM数据缓冲区 */

/**
 * @description: 将RGB颜色数据转换为WS2812所需的PWM波形数据
 * @param {uint8_t} red 红色分量（0-255）
 * @param {uint8_t} green 绿色分量（0-255）
 * @param {uint8_t} blue 蓝色分量（0-255）
 * @param {uint16_t} pwm_data PWM数据数组指针
 * @param {int} led_index LED索引号（0-LED_COUNT-1）
 * @return {void}
 */
void convert_color_to_pwm(uint8_t red, uint8_t green, uint8_t blue, uint16_t pwm_data[], int led_index) 
{
    int base_index = led_index * 3 * 8;  /* 存储该LED在PWM数据中的起始位置 */
    int bit_value;

    // 绿色分量 (GRB顺序)
    for (int i = 0; i < 8; i++) 
    {
        bit_value = (green >> (7 - i)) & 0X01;      /* 从高位到低位提取每一位 */
        if (bit_value == 1) {
            pwm_data[base_index + i] = HIGH_PULSE;  /* 数据位1对应高脉冲 */
        } else {
            pwm_data[base_index + i] = LOW_PULSE;   /* 数据位0对应低脉冲 */
        }
    }

    // 红色分量
    for (int i = 0; i < 8; i++) 
    {
        bit_value = (red >> (7 - i)) & 0X01;
        if (bit_value == 1) {
            pwm_data[base_index + 8 + i] = HIGH_PULSE;
        } else {
            pwm_data[base_index + 8 + i] = LOW_PULSE;
        }
    }

    // 蓝色分量
    for (int i = 0; i < 8; i++) 
    {
        bit_value = (blue >> (7 - i)) & 1;
        if (bit_value == 1) {
            pwm_data[base_index + 2 * 8 + i] = HIGH_PULSE;
        } else {
            pwm_data[base_index + 2 * 8 + i] = LOW_PULSE;
        }
    }
}

/**
 * @description: 更新LED显示，通过DMA传输PWM数据到定时器
 * @param {void}
 * @return {void}
 */
void update_reveal(void)
{
    // 先启动前灯DMA传输
    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwm_data_frontled, sizeof(pwm_data_frontled) / sizeof(pwm_data_frontled[0]));
    while (HAL_DMA_GetState(&hdma_tim1_ch1) != HAL_DMA_STATE_READY){}  /* 等待DMA传输完成 */
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);

    // 再启动后灯DMA传输
    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_4, (uint32_t *)pwm_data_rearled, sizeof(pwm_data_rearled) / sizeof(pwm_data_rearled[0]));
    while (HAL_DMA_GetState(&hdma_tim1_ch4_trig_com) != HAL_DMA_STATE_READY){}  /* 等待DMA传输完成 */
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_4);
}

/**
 * @description: 清空PWM数据缓冲区
 * @param {uint16_t} pwm_data PWM数据数组指针
 * @return {void}
 */
void clear_pwm_data(uint16_t pwm_data[])
{
    int i;
    
    // 清空LED数据部分（设置为低电平）
    for (i = 0; i < DATA_SIZE; i++) 
    {
        pwm_data[i] = LOW_PULSE;
    }
    
    // 设置复位部分（设置为0，产生长低电平复位信号）
    for (i = DATA_SIZE; i < DATA_SIZE + WS2812_RST_NUM; i++)
    {
        pwm_data[i] = 0;
    }
}

/**
 * @description: 设置单个LED的颜色
 * @param {int} led_index LED索引号（0-LED_COUNT-1）
 * @param {uint8_t} red 红色分量（0-255）
 * @param {uint8_t} green 绿色分量（0-255）
 * @param {uint8_t} blue 蓝色分量（0-255）
 * @param {uint16_t} pwm_data PWM数据数组指针
 * @return {void}
 */
void set_led_color(int led_index, uint8_t red, uint8_t green, uint8_t blue, uint16_t pwm_data[]) 
{
    /* 参数有效性检查 */
    if (led_index < 0) 
    {
        led_index = 0;
    }
    if (led_index >= LED_COUNT) 
    {
        led_index = LED_COUNT - 1;
    }
    
    clear_pwm_data(pwm_data);
    convert_color_to_pwm(red, green, blue, pwm_data, led_index);
    update_reveal();
}

/**
 * @description: 设置多个连续LED的颜色
 * @param {uint8_t} start_index 起始LED索引
 * @param {uint8_t} end_index 结束LED索引
 * @param {uint8_t} red 红色分量（0-255）
 * @param {uint8_t} green 绿色分量（0-255）
 * @param {uint8_t} blue 蓝色分量（0-255）
 * @param {uint16_t} pwm_data PWM数据数组指针
 * @return {void}
 */
void set_multiple_leds(uint8_t start_index, uint8_t end_index, uint8_t red, uint8_t green, uint8_t blue, uint16_t pwm_data[]) 
{
    int i;
    
    /* 参数有效性检查 */
    if (start_index >= LED_COUNT) 
    {
        start_index = LED_COUNT - 1;
    }
    if (end_index >= LED_COUNT) 
    {
        end_index = LED_COUNT - 1;
    }
    if (start_index > end_index) 
    {
        return;  /* 起始索引大于结束索引，直接返回 */
    }
    
    clear_pwm_data(pwm_data);
    for (i = start_index; i <= end_index; i++) 
    {
        convert_color_to_pwm(red, green, blue, pwm_data, i);
    }
    update_reveal();
}

/**
 * @description: 流水灯效果，LED依次点亮
 * @param {uint8_t} red 红色分量（0-255）
 * @param {uint8_t} green 绿色分量（0-255）
 * @param {uint8_t} blue 蓝色分量（0-255）
 * @param {uint16_t} pwm_data PWM数据数组指针
 * @return {void}
 */
void flowLight(uint8_t red, uint8_t green, uint8_t blue, uint16_t pwm_data[])
{
    int i;
    
    for (i = 0; i < LED_COUNT; i++)
    {
        set_led_color(i, red, green, blue, pwm_data);
        HAL_Delay(50);  /* 每个LED点亮间隔50ms */
    }
		
		// 关闭所有灯光 
		set_multiple_leds(0, 27, 0, 0, 0, pwm_data); 
}

/**
 * @description: 呼吸灯效果，LED亮度渐变
 * @param {uint8_t} start_index 起始LED索引
 * @param {uint8_t} end_index 结束LED索引
 * @param {uint8_t} red 红色分量（0-255）
 * @param {uint8_t} green 绿色分量（0-255）
 * @param {uint8_t} blue 蓝色分量（0-255）
 * @param {uint16_t} pwm_data PWM数据数组指针
 * @return {void}
 */
void breathLight(uint8_t start_index, uint8_t end_index, uint8_t red, uint8_t green, uint8_t blue, uint16_t pwm_data[])
{
    uint8_t brightness = 0;  /* 当前亮度值 */
    int direction = 1;       /* 亮度变化方向：1变亮，0变暗 */
    uint8_t current_red, current_green, current_blue;  /* 当前亮度对应的颜色值 */
    
    /* 参数有效性检查 */
    if (start_index >= LED_COUNT) 
    {
        start_index = LED_COUNT - 1;
    }
    if (end_index >= LED_COUNT) 
    {
        end_index = LED_COUNT - 1;
    }
    if (start_index > end_index) 
    {
        return;
    }
    
    clear_pwm_data(pwm_data);
    
    while (1)
    {
        if (direction == 1)  /* 变亮过程 */
        {
            brightness = brightness + 1;
            if (brightness >= 255)
            {
                direction = 0;     /* 达到最大亮度，开始变暗 */
                brightness = 255;
            }
        }
        else  /* 变暗过程 */
        {
            brightness = brightness - 1;
            if (brightness <= 0)
            {
                // 关闭所有灯光 
                set_multiple_leds(0, 27, 0, 0, 0, pwm_data); 
                break;  /* 亮度为0，退出循环 */
            }
        }    
        
        // 计算当前亮度下的颜色（线性调光）
        current_red = (uint8_t)(red * brightness / 255);
        current_green = (uint8_t)(green * brightness / 255);
        current_blue = (uint8_t)(blue * brightness / 255);
        
        set_multiple_leds(start_index, end_index, current_red, current_green, current_blue, pwm_data);
        HAL_Delay(1);  /* 亮度更新间隔1ms */
    }
}

/**
 * @description: 从中间向两边的流水灯效果
 * @param {uint8_t} red 红色分量（0-255）
 * @param {uint8_t} green 绿色分量（0-255）
 * @param {uint8_t} blue 蓝色分量（0-255）
 * @param {uint16_t} pwm_data PWM数据数组指针
 * @return {void}
 */
void flow_from_middle(uint8_t red, uint8_t green, uint8_t blue, uint16_t pwm_data[]) 
{
    int middle = LED_COUNT / 2;  /* 中间位置 */
    int left = middle - 1;      /* 左侧起始位置 */
    int right = middle;         /* 右侧起始位置 */

    clear_pwm_data(pwm_data);

    // 从中间向两边点亮
    while (left >= 0 && right < LED_COUNT) 
    {
        convert_color_to_pwm(red, green, blue, pwm_data, left);
        convert_color_to_pwm(red, green, blue, pwm_data, right);
        update_reveal();
        HAL_Delay(50);  /* 每次点亮间隔50ms */
        
        left = left - 1;   /* 向左移动 */
        right = right + 1; /* 向右移动 */
    }

    // 从两边向中间熄灭
    left = 0;
    right = LED_COUNT - 1;
    while (left < right) 
    {
        convert_color_to_pwm(0, 0, 0, pwm_data, left);     /* 熄灭左侧LED */
        convert_color_to_pwm(0, 0, 0, pwm_data, right);    /* 熄灭右侧LED */
        update_reveal();
        HAL_Delay(50);
        left = left + 1;   /* 向中间移动 */
        right = right - 1; /* 向中间移动 */
    }
}
