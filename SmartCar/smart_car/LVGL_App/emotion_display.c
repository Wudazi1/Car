#include "lvgl.h"
#include "font_awesome_symbols.h"
#include "font_awesome_30_1.h"
#include <string.h>
#include <stdio.h>

static lv_obj_t* emotion_label = NULL;

// 表情映射表
static const struct {
    const char* name;
    const char* icon;
} emotion_map[] = {
    {"neutral", FONT_AWESOME_EMOJI_NEUTRAL},
    {"happy", FONT_AWESOME_EMOJI_HAPPY},
    {"laughing", FONT_AWESOME_EMOJI_LAUGHING},
    {"funny", FONT_AWESOME_EMOJI_FUNNY},
    {"sad", FONT_AWESOME_EMOJI_SAD},
    {"angry", FONT_AWESOME_EMOJI_ANGRY},
    {"crying", FONT_AWESOME_EMOJI_CRYING},
    {"loving", FONT_AWESOME_EMOJI_LOVING},
    {"surprised", FONT_AWESOME_EMOJI_SURPRISED},
    {"shocked", FONT_AWESOME_EMOJI_SHOCKED},
    {"thinking", FONT_AWESOME_EMOJI_THINKING},
    {"cool", FONT_AWESOME_EMOJI_COOL},
    {"sleepy", FONT_AWESOME_EMOJI_SLEEPY}
};

#define EMOTION_COUNT (sizeof(emotion_map)/sizeof(emotion_map[0]))

void emotion_display_init(void)
{
    emotion_label = lv_label_create(lv_scr_act());
    lv_obj_set_size(emotion_label, 48, 32);  // 增加宽度
    lv_obj_set_pos(emotion_label, 40, 0);  // 居中显示
    lv_obj_set_style_text_font(emotion_label, &font_awesome_30_1, 0);
    lv_label_set_text(emotion_label, FONT_AWESOME_EMOJI_NEUTRAL);
    lv_obj_set_style_text_align(emotion_label, LV_TEXT_ALIGN_CENTER, 0);  // 文本居中
}

void set_emotion(const char* emotion)
{
    if (emotion_label == NULL) return;
    
    for (int i = 0; i < EMOTION_COUNT; i++) {
        if (strcmp(emotion, emotion_map[i].name) == 0) {
            lv_label_set_text(emotion_label, emotion_map[i].icon);
            return;
        }
    }
    
    lv_label_set_text(emotion_label, FONT_AWESOME_EMOJI_NEUTRAL);
}

void extract_emoji_bitmap(void)
{
    uint32_t emoji_codes[] = {
        0xF5A4,  // neutral
        0xF118,  // happy
        0xF59B,  // laughing
        0xF588,  // funny
        0xE384,  // sad
        0xF556,  // angry
        0xF5B3,  // crying
        0xF584,  // loving
        0xE36B,  // surprised
        0xE375,  // shocked
        0xE39B,  // thinking
        0xE398,  // cool
        0xE38D   // sleepy
    };
    const char *names[] = {
        "neutral", "happy", "laughing", "funny", "sad", "angry",
        "crying", "loving", "surprised", "shocked", "thinking",
        "cool", "sleepy"
    };

    const lv_font_t *font = &font_awesome_30_1;   
    lv_font_glyph_dsc_t dsc;
    const uint8_t *bitmap;

    for (int i = 0; i < 13; i++) {
        if (lv_font_get_glyph_dsc(font, &dsc, emoji_codes[i], 0)) {
            bitmap = lv_font_get_glyph_bitmap(font, emoji_codes[i]);
            int bytes = (dsc.box_w * dsc.box_h + 7) / 8;
            printf("\n// %s, size %dx%d, bytes %d\n", names[i], dsc.box_w, dsc.box_h, bytes);
            printf("const uint8_t %s_bits[%d] = {", names[i], bytes);
            for (int j = 0; j < bytes; j++) {
                if (j % 16 == 0) printf("\n    ");
                printf("0x%02x, ", bitmap[j]);
            }
            printf("\n};\n");
        } else {
            printf("// Failed to get glyph for %s (0x%X)\n", names[i], emoji_codes[i]);
        }
    }
}
