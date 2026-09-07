#include "track.h"
#include "stdio.h"

void track_proc(void)
{
    uint8_t track0, track1, track2, track3;
    
    track0 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10);
    track1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11);
    track2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12);
    track3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13);
    
    // 直行 0 1 1 0
    if(!track0 && track1 && track2 && track3)
    {
        servo_set(0, 90);
        servo_set(1, 90);
        front_right_set_speed(1, 45);
        front_left_set_speed(1, 45);
        rear_right_set_speed(1, 45);
        rear_left_set_speed(1, 45);
        
        set_multiple_leds(0, 27, 255, 255, 255, pwm_data_frontled);
        set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
    }
    // 小左转 0 1 0 0
    else if(!track0 && track1 && !track2 && !track3)
    {
        servo_set(0, 50);
        servo_set(1, 90);
        front_right_set_speed(1, 45);
        front_left_set_speed(1, 45);
        rear_right_set_speed(1, 45);
        rear_left_set_speed(1, 45);
        
        set_multiple_leds(0, 13, 255, 255, 0, pwm_data_frontled);
        set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
    }
    // 小右转 0 0 1 0
    else if(!track0 && !track1 && track2 && !track3)
    {
        servo_set(0, 130);
        servo_set(1, 90);
        
        front_right_set_speed(1, 45);
        front_left_set_speed(1, 45);
        rear_right_set_speed(1, 45);
        rear_left_set_speed(1, 45);
        
        set_multiple_leds(14, 27, 255, 255, 0, pwm_data_frontled);
        set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
    }
    // 大左转 1 0 0 0
    else if(track0 && !track1 && !track2 && !track3)
    {
        servo_set(0, 0);
        servo_set(1, 90);
        
        front_right_set_speed(1, 48);
        front_left_set_speed(1, 48);
        rear_right_set_speed(1, 48);
        rear_left_set_speed(1, 48);
        
        set_multiple_leds(0, 13, 255, 255, 0, pwm_data_frontled);
        set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
    }
    // 大右转 0 0 0 1
    else if(!track0 && !track1 && !track2 && track3)
    {
        servo_set(0, 180);
        servo_set(1, 90);
        
        front_right_set_speed(1, 48);
        front_left_set_speed(1, 48);
        rear_right_set_speed(1, 48);
        rear_left_set_speed(1, 48);
        
        set_multiple_leds(14, 27, 255, 255, 0, pwm_data_frontled);
        set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
    }
    // 停车 1 1 1 1
    else if(track0 && track1 && track2 && track3)
    {
        front_right_set_speed(1, 0);
        front_left_set_speed(1, 0);
        rear_right_set_speed(1, 0);
        rear_left_set_speed(1, 0);
        
        set_multiple_leds(0, 27, 255, 255, 255, pwm_data_frontled);
        set_multiple_leds(0, 27, 255, 0, 0, pwm_data_rearled);
    }
    // 前进 0 0 0 0
    else if(!track0 && track1 && !track2 && ! track3)
    {
        front_right_set_speed(1, 45);
        front_left_set_speed(1, 45);
        rear_right_set_speed(1, 45);
        rear_left_set_speed(1, 45);
        
        set_multiple_leds(0, 27, 255, 255, 255, pwm_data_frontled);
        set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
    }
}
