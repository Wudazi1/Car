#include "track.h"
#include "stdio.h"

void track_proc(void)
{
    uint8_t track0 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10);
    uint8_t track1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11);
    uint8_t track2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12);
    uint8_t track3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13);

    uint8_t pattern = (track0 << 3) | (track1 << 2) | (track2 << 1) | (track3 << 0);

    switch(pattern) {
        case TRACK_STRAIGHT:
            servo_set(0, 90);
            servo_set(1, 90);
            front_right_set_speed(1, 45);
            front_left_set_speed(1, 45);
            rear_right_set_speed(1, 45);
            rear_left_set_speed(1, 45);
            set_multiple_leds(0, 27, 255, 255, 255, pwm_data_frontled);
            set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
            break;

        case TRACK_LEFT_SMALL:
            servo_set(0, 50);
            servo_set(1, 90);
            front_right_set_speed(1, 45);
            front_left_set_speed(1, 45);
            rear_right_set_speed(1, 45);
            rear_left_set_speed(1, 45);
            set_multiple_leds(0, 13, 255, 255, 0, pwm_data_frontled);
            set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
            break;

        case TRACK_RIGHT_SMALL:
            servo_set(0, 130);
            servo_set(1, 90);
            front_right_set_speed(1, 45);
            front_left_set_speed(1, 45);
            rear_right_set_speed(1, 45);
            rear_left_set_speed(1, 45);
            set_multiple_leds(14, 27, 255, 255, 0, pwm_data_frontled);
            set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
            break;

        case TRACK_LEFT_BIG:
            servo_set(0, 0);
            servo_set(1, 90);
            front_right_set_speed(1, 48);
            front_left_set_speed(1, 48);
            rear_right_set_speed(1, 48);
            rear_left_set_speed(1, 48);
            set_multiple_leds(0, 13, 255, 255, 0, pwm_data_frontled);
            set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
            break;

        case TRACK_RIGHT_BIG:
            servo_set(0, 180);
            servo_set(1, 90);
            front_right_set_speed(1, 48);
            front_left_set_speed(1, 48);
            rear_right_set_speed(1, 48);
            rear_left_set_speed(1, 48);
            set_multiple_leds(14, 27, 255, 255, 0, pwm_data_frontled);
            set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
            break;

        case TRACK_STOP:
            front_right_set_speed(1, 0);
            front_left_set_speed(1, 0);
            rear_right_set_speed(1, 0);
            rear_left_set_speed(1, 0);
            set_multiple_leds(0, 27, 255, 255, 255, pwm_data_frontled);
            set_multiple_leds(0, 27, 255, 0, 0, pwm_data_rearled);
            break;

        case TRACK_GO:
            front_right_set_speed(1, 45);
            front_left_set_speed(1, 45);
            rear_right_set_speed(1, 45);
            rear_left_set_speed(1, 45);
            set_multiple_leds(0, 27, 255, 255, 255, pwm_data_frontled);
            set_multiple_leds(0, 27, 0, 0, 0, pwm_data_rearled);
            break;

        default:
            break;
    }
}
