#ifndef __SERVO_H
#define __SERVO_H

#include "tim.h"
#include "gpio.h"

void servo_set(uint16_t angle);
void servo_init(void);

#endif
