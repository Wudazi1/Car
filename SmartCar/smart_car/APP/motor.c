#include "motor.h"

void motor_init(void)
{
	// motor1
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	// motor2
	HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
	// motor3
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);
	// motor4
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
}

void front_right_set_speed(uint8_t dir, uint16_t speed)
{
	if(speed > 100) speed = 100;
	
	if(dir)
	{
		// go forward IN1 = 0 IN2 = 1
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
	}
	else
	{
		// back up IN1 = 1 IN2 = 0
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
	}
}

void front_left_set_speed(uint8_t dir, uint16_t speed)
{
	if(speed > 100) speed = 100;
	
	if(dir)
	{
		// go forward IN1 = 0 IN2 = 1
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, speed);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
	}
	else
	{
		// back up IN1 = 1 IN2 = 0
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, speed);
	}
}

void rear_right_set_speed(uint8_t dir, uint16_t speed)
{
	if(speed > 100) speed = 100;
	
	if(dir)
	{
		// go forward IN1 = 0 IN2 = 1
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, speed);
		__HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, 0);
	}
	else
	{
		// back up IN1 = 1 IN2 = 0
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, speed);
	}
}

void rear_left_set_speed(uint8_t dir, uint16_t speed)
{
	if(speed > 100) speed = 100;
	
	if(dir)
	{
		// go forward IN1 = 0 IN2 = 1
		__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_2, speed);
		__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 0);
	}
	else
	{
		// back up IN1 = 1 IN2 = 0
		__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_2, 0);
		__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, speed);
	}
}
