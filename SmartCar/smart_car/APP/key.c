#include "key.h"

char buf[20];

uint8_t key_val;		// 当前按键状态值

uint8_t key_read(void)
{
	uint8_t temp = 0;
	
	if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3) == GPIO_PIN_RESET) temp = 1;
	if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4) == GPIO_PIN_RESET) temp = 2;
	
	  if(temp != 0)
    {
        HAL_Delay(20);
        if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3) == GPIO_PIN_RESET) temp = 1;
        else if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4) == GPIO_PIN_RESET) temp = 2;
        else temp = 0;
    }
		
	return temp;
}

void key_proc(void)
{
	// 获取当前按键状态
	key_val = key_read();
	if(key_val == 1) HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_7);
	if(key_val == 2) HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_7);
}
