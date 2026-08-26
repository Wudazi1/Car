#ifndef EMOTION_H
#define EMOTION_H

#include <stdint.h>

// 声明所有位图数组
extern const uint8_t neutral_bits[113];
extern const uint8_t happy_bits[113];
extern const uint8_t laughing_bits[113];
extern const uint8_t funny_bits[143];
extern const uint8_t sad_bits[113];
extern const uint8_t angry_bits[113];
extern const uint8_t crying_bits[113];
extern const uint8_t loving_bits[113];
extern const uint8_t surprised_bits[113];
extern const uint8_t shocked_bits[113];
extern const uint8_t thinking_bits[113];
extern const uint8_t cool_bits[113];
extern const uint8_t sleepy_bits[113];

// 每个表情的尺寸（宽、高）
#define NEUTRAL_W  30
#define NEUTRAL_H  30
#define HAPPY_W    30
#define HAPPY_H    30
#define LAUGHING_W 30
#define LAUGHING_H 30
#define FUNNY_W    38
#define FUNNY_H    30
#define SAD_W      30
#define SAD_H      30
#define ANGRY_W    30
#define ANGRY_H    30
#define CRYING_W   30
#define CRYING_H   30
#define LOVING_W   30
#define LOVING_H   30
#define SURPRISED_W 30
#define SURPRISED_H 30
#define SHOCKED_W   30
#define SHOCKED_H   30
#define THINKING_W  30
#define THINKING_H  30
#define COOL_W      30
#define COOL_H      30
#define SLEEPY_W    30
#define SLEEPY_H    30

void set_emotion(const char* emotion);

#endif
