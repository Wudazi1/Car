#include "servo.h"

uint16_t count = 0,front_pwm_value = 0,back_pwm_value = 0;

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



// timer interrupt entry function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)  // timer6 10us
	{
		// front pwm output
		if(++count == 2000) count = 0;
		if(count < front_pwm_value)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
		}
		// back pwm output
		if(count < back_pwm_value)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
		}
	}
}
