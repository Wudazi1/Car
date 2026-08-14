#ifndef __MOTOR_H
#define __MOTOR_H

#include "tim.h"
#include "gpio.h"

void motor_init(void);
void front_right_set_speed(uint8_t dir, uint16_t speed);
void front_left_set_speed(uint8_t dir, uint16_t speed);
void rear_right_set_speed(uint8_t dir, uint16_t speed);
void rear_left_set_speed(uint8_t dir, uint16_t speed);

#endif
