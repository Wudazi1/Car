#ifndef __WS2812_H
#define __WS2812_H

#include "main.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

// 定义LED数量
#define LED_COUNT 28                   /* LED灯珠数量 */
#define DATA_SIZE (LED_COUNT * 3 * 8)  /* PWM数据大小：LED数量×3颜色×8位/颜色 */
/* DUTY（占空比）= CCR/(ARR+1) ，且ARR+1 = 90，0码->1/3的占空比，1码->2/3的占空比*/ 
#define HIGH_PULSE 60                  /* 高电平脉冲宽度（对应数据位1）*/
#define LOW_PULSE 30                   /* 低电平脉冲宽度（对应数据位0）*/
/* 低电平时间需大于125us，1.25us x 300远大于125us*/
#define WS2812_RST_NUM 300             /* WS2812复位脉冲数量，低电平时间需大于125us，1.25us x 300远大于125us*/

// 定义PWM数据数组
extern uint16_t pwm_data_frontled[DATA_SIZE + WS2812_RST_NUM];  /* 前灯PWM数据缓冲区 */
extern uint16_t pwm_data_rearled[DATA_SIZE + WS2812_RST_NUM];   /* 后灯PWM数据缓冲区 */

void set_led_color(int led_index, uint8_t red, uint8_t green, uint8_t blue, uint16_t pwm_data[]);
void flowLight(uint8_t red, uint8_t green, uint8_t blue, uint16_t pwm_data[]);
void breathLight(uint8_t start_index, uint8_t end_index, uint8_t red, uint8_t green, uint8_t blue, uint16_t pwm_data[]);
void flow_from_middle(uint8_t red, uint8_t green, uint8_t blue, uint16_t pwm_data[]) ;

#endif
