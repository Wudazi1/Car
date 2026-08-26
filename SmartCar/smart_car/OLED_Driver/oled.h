#ifndef __OLED_H
#define __OLED_H

#include "system_bsp.h" 

extern const uint8_t pic[];

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ClearPage(uint8_t page);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr);
void OLED_ShowString(uint8_t x, uint8_t y, char *str);
void OLED_ShowRowString(uint8_t x, uint8_t y, char *str);
void OLED_ShowLineString(uint8_t x, uint8_t y, char *str);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
void OLED_ShowHexNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
void OLED_ShowBinNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2);
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t no);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[]);
void OLED_ShowPicture(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t *pic);

void OLED_Printf(uint8_t x, uint8_t y, const char *fmt, ...);

void OLED_DrawBitmap_Raw(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap);

#endif
