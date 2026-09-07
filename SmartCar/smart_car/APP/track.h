#ifndef __TRACK_H
#define __TRACK_H

#include "system_bsp.h"

// ==================== 传感器组合定义 ==================== //
// bit3=track0, bit2=track1, bit1=track2, bit0=track3
#define TRACK_STRAIGHT    0x6   // 0 1 1 0 直行
#define TRACK_LEFT_SMALL  0x4   // 0 1 0 0 小左转
#define TRACK_RIGHT_SMALL 0x2  // 0 0 1 0 小右转
#define TRACK_LEFT_BIG    0x8   // 1 0 0 0 大左转
#define TRACK_RIGHT_BIG   0x1  // 0 0 0 1 大右转
#define TRACK_STOP        0xF   // 1 1 1 1 停车
#define TRACK_GO          0x0   // 0 0 0 0 前进

void track_proc(void);

#endif
