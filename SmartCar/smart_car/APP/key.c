#include "key.h"

uint8_t key_val, key_down, key_old;

uint8_t key_read(void)
{
    static uint8_t last_raw = 0, stable = 0, cnt = 0;
	uint8_t raw = 0;
    
	if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3) == GPIO_PIN_RESET) raw |= 0x01;
	if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4) == GPIO_PIN_RESET) raw |= 0x02;
	
	if(raw == last_raw)
    {
        if(cnt < 2) cnt++; 
    } else
    {
        cnt = 0;
        last_raw = raw;
    }
    
    if(cnt >= 2) stable = raw;
		
	return stable;
}

static uint32_t press_start = 0; // 按下开始时间
static uint8_t long_press_triggered = 0; // 长按触发标志

void key_proc(void)
{
    key_val = key_read();
    key_down = key_val & (key_val ^ key_old);
    key_old = key_val;
    
    if(key_down == 1)
    {
        
    }
    
    if(key_down ==2)
    {
        
    }
    
    if(key_val == 3)
    {
        if(press_start == 0)
        {
            press_start = HAL_GetTick();
            long_press_triggered = 0;
        }
        else if(!long_press_triggered && (HAL_GetTick() - press_start) >= 2000)
        {
            // 长按超过2秒执行逻辑 
            long_press_triggered = 1;  // 标记已触发
        }
    }
    else
    {
        press_start = 0;
        long_press_triggered = 0;
    }
    
}
