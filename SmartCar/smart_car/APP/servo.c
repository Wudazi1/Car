#include "servo.h"

uint16_t count = 0,pwm_value = 0;

void servo_set(uint16_t angle)
{
	if(angle>180)    angle = 180;
	
	pwm_value = 50 + (angle * 200) / 180;
}

void servo_init(void)
{
	// start time6
	HAL_TIM_Base_Start_IT(&htim6);
	
	servo_set(90);
}



// timer interrupt entry function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)  // timer6 10us
	{
		// pwm output
		if(++count == 2000) count = 0;
		if(count < pwm_value)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
		}
	}
}
