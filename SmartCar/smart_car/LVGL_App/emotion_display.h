#ifndef EMOTION_DISPLAY_H
#define EMOTION_DISPLAY_H

#include <stdint.h>

// 初始化表情显示
void emotion_display_init(void);

// 设置表情
void set_emotion(const char* emotion);

void extract_emoji_bitmap(void);

#endif // EMOTION_DISPLAY_H
