#ifndef __SERVO_H
#define __SERVO_H

#include "tim.h"
#include "gpio.h"

void servo_set(int dir,uint16_t angle);
void servo_init(void);

extern uint16_t front_pwm_value;
extern uint16_t back_pwm_value;

#endif
