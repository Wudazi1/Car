#include "servo.h"

uint16_t front_pwm_value = 0,back_pwm_value = 0;

void servo_set(int dir,uint16_t angle)
{
	if(angle>180)    angle = 180;
	if(dir == 0)
	{
		front_pwm_value = 50 + (angle * 200) / 180;
	}
	else if(dir == 1)
	{
		back_pwm_value = 50 + (angle * 200) / 180;
	}
	
}

void servo_init(void)
{
	// start time6
	HAL_TIM_Base_Start_IT(&htim6);
	
	servo_set(0,90);
	servo_set(1,90);
}
