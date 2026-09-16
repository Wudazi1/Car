#ifndef LVGL_PORT_H
#define LVGL_PORT_H

#include <stdint.h>

// 初始化LVGL显示驱动
void lvgl_port_init(void);

// LVGL任务处理（在主循环中调用）
void lvgl_task_handler(void);

#endif // LVGL_PORT_H
