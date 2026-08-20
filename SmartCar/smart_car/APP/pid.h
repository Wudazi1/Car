#ifndef __PID_H
#define __PID_H

#include "main.h"

extern int16_t target_rpm[4];

extern float pid_kp;
extern float pid_ki;
extern float pid_kd;

void pid_proc(void);

#endif
