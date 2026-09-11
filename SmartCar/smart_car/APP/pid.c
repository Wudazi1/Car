//*****************************************************************//
// Feature:				PID Controller
// Author:				WuYunlong
//*****************************************************************//

#include "pid.h"
#include "encoder.h"
#include "motor.h"
#include "servo.h"
#include <math.h>

// PID parameters(global variables)
float pid_kp = 100.0f;
float pid_ki = 150.0f;
float pid_kd = 0.0f;

// optimization configurations
#define OUTPUT_DEADZONE    10
#define INTEGRAL_SEP_THRESH 2.0f
#define DIFF_FILTER_ALPHA   0.1f

// PID state structure
typedef struct {
    float prev_error;
    float integral;
    uint32_t last_pid_time;
    float last_target;
    float prev_derivative;
} PID_State;

// Function pointer type definitions
typedef float (*GetSpeedFunc)(void);
typedef void (*SetSpeedFunc) (uint8_t, uint16_t);

// PID state array
static PID_State pid_state[4] = {0};
// global target speed (m/s)
float target_speed[4] = {0.0f, 0.0f, 0.0f, 0.0f};

/**
 * @brief Single motor PID control function - Position-form algorithm
 * @param motor_id: Motor ID (0-3)
 * @param target_speed: Target speed (m/s)
 * @param get_speed: Function pointer to read current speed (m/s)
 * @param set_speed: Function pointer to set motor speed and direction
 */
static void motor_PID(uint8_t motor_id, float target_speed, GetSpeedFunc get_speed, SetSpeedFunc set_speed)
{
    // 1. calculate time interval
    uint32_t current_time = HAL_GetTick();
    float dt = (float)(current_time - pid_state[motor_id].last_pid_time) / 1000.0f;

    if(pid_state[motor_id].last_pid_time == 0)
    {
        pid_state[motor_id].last_pid_time = current_time;
        pid_state[motor_id].last_target = target_speed;
        return;
    }

    if(dt <= 0 || dt > 0.1f)    dt = 0.01f;

    // 2. target value change detection
    if(fabsf(target_speed - pid_state[motor_id].last_target) > 2.0f)
    {
        pid_state[motor_id].integral = 0.0f;
        pid_state[motor_id].prev_derivative = 0.0f;
    }
    pid_state[motor_id].last_target = target_speed;

    // 3. read current speed and calculate error
    // 使用绝对值计算PID，方向由target_speed符号决定
    float current_speed = get_speed();
    float abs_target = fabsf(target_speed);
    float abs_current = fabsf(current_speed);
    float error = abs_target - abs_current;

    // 4. calculate proportional term
    float P = pid_kp * error;
    // 5. claculate integral term
    float I = pid_ki * pid_state[motor_id].integral;
    // 6. calculate derivative term
    float raw_D = pid_kd * (error - pid_state[motor_id].prev_error) / dt;
    float D = DIFF_FILTER_ALPHA * raw_D + (1.0f - DIFF_FILTER_ALPHA) * pid_state[motor_id].prev_derivative;
    pid_state[motor_id].prev_derivative = D;

    // derivative term limiting
    if(D > 50.0f) D = 50.0f;
    if(D < -50.0f) D = -50.0f;

    // 7. colculate total output and clamp
    float output = P + I + D;
    if(output > 200.0f) output = 200.0f;
    if(output < 0.0f) output = 0.0f;

    // 8. output deadzone + direction
    int final_dir = (target_speed >= 0) ? 1 : 0;
    uint16_t final_speed = 0;

    if(abs_target < 0.01f)
    {
        // 目标是停止
        final_speed = 0;
    }
    else if(fabsf(output) < OUTPUT_DEADZONE)
    {
        // 速度太小，给个最小输出
        final_speed = OUTPUT_DEADZONE;
    }
    else
    {
        final_speed = (uint16_t)(output);
    }

    // apply output
    set_speed(final_dir, final_speed);

    // 9. integral accumulation
    if(fabsf(error) < INTEGRAL_SEP_THRESH)
    {
        if(!(output >= 200.0f && error > 0))
        {
            pid_state[motor_id].integral += error * dt;
        }
    }

    // 10. integral limit
    if(pid_state[motor_id].integral > 200.0f) pid_state[motor_id].integral = 200.0f;
    if(pid_state[motor_id].integral < 0.0f) pid_state[motor_id].integral = 0.0f;

    // 11. save state
    pid_state[motor_id].prev_error = error;
    pid_state[motor_id].last_pid_time = current_time;
    
    // 调试pid时VOFA看图使用
//    printf("%d,%.2f,%.2f,%.2f\r\n", motor_id, target_speed, current_speed, output);
}

/**
 * @brief PID processing function for controlling all four motors
 */
void pid_proc(void)
{
    if(target_speed[0] != 0.0f || target_speed[1] != 0.0f ||
       target_speed[2] != 0.0f || target_speed[3] != 0.0f)
    {
        motor_PID(0, target_speed[0], encoder1_getspeed_ms, front_right_set_speed);
        motor_PID(1, target_speed[1], encoder2_getspeed_ms, front_left_set_speed);
        motor_PID(2, target_speed[2], encoder3_getspeed_ms, rear_right_set_speed);
        motor_PID(3, target_speed[3], encoder4_getspeed_ms, rear_left_set_speed);
    }
    else
    {
        front_right_set_speed(1, 0);
        front_left_set_speed(1, 0);
        rear_right_set_speed(1, 0);
        rear_left_set_speed(1, 0);
    }
}

// ==================== 运动控制函数 ==================== //

/**
 * @brief 设置速度（正数前进，负数倒车，零停止）
 * @param speed_ms: 速度 m/s
 */
void car_set_speed(float speed_ms)
{
    target_speed[0] = target_speed[1] = target_speed[2] = target_speed[3] = speed_ms;
}
