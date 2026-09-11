#ifndef __PID_H
#define __PID_H

#include "main.h"

extern float target_speed[4];  // 目标速度，单位 m/s

extern float pid_kp;
extern float pid_ki;
extern float pid_kd;

void pid_proc(void);

// 运动控制函数
void car_set_speed(float speed_ms);   // 正数前进，负数倒车，零停止

#endif
