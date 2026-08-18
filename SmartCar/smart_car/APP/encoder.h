#ifndef __ENCODER_H
#define __ENCODER_H

#include "tim.h"
#include "gpio.h"

void encoder_init(void);
int16_t encoder1_read(void);
int encoder1_getrpm_smooth(void);
int16_t encoder2_read(void);
int encoder2_getrpm_smooth(void);
int16_t encoder3_read(void);
int encoder3_getrpm_smooth(void);
int16_t encoder4_read(void);
int encoder4_getrpm_smooth(void);

// 速度计算相关函数
int get_average_rpm(void);
float rpm_to_speed_ms(int rpm);
float get_robot_speed(void);

// 里程计算相关函数
void update_motor1_odometer(void);
void update_motor2_odometer(void);
void update_motor3_odometer(void);
void update_motor4_odometer(void);
void update_all_odometer(void);
uint32_t get_average_total_rev(void);

#endif
