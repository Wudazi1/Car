#include "lvgl_port.h"
#include "lvgl.h"
#include "oled.h"
#include "stm32f1xx_hal.h"
#include <string.h>

// LVGL显示缓冲区 - 使用 lv_color_t 数组，每个元素对应一个像素
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf_1[128 * 32];   // 4096字节，存储未压缩像素数据
static lv_color_t buf_2[128 * 32];   // 另一个4KB，可选双缓冲（可改为NULL节省内存）

// OLED压缩帧缓冲：128x32像素压缩为 128*4 = 512字节（每字节8像素）
static uint8_t oled_compressed_buf[128 * 4];

/**
  * @brief  LVGL刷新回调函数（全屏刷新模式）
  * @note   将LVGL的未压缩像素数据转换为OLED的压缩页格式并写入屏幕
  */
static void ssd1306_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    // 将LVGL的未压缩像素数据转换为OLED的压缩页格式
    memset(oled_compressed_buf, 0, sizeof(oled_compressed_buf));

    // 遍历需要刷新的区域（即使启用 full_refresh，area 也可能等于整个屏幕）
    for (uint16_t y = area->y1; y <= area->y2; y++) {
        for (uint16_t x = area->x1; x <= area->x2; x++) {
            uint16_t pixel_index = y * 128 + x;
            uint8_t pixel = color_map[pixel_index].full & 1;  // 取最低位作为像素值

            if (!pixel) {   // 取反：LVGL 认为熄灭时，我们点亮
                uint8_t page = y / 8;
                uint8_t bit = y % 8;
                oled_compressed_buf[page * 128 + x] |= (1 << bit);
            }
        }
    }

    // 将整个压缩缓冲区发送到OLED
    for (uint8_t page = 0; page < 4; page++) {
        OLED_Set_Pos(0, page);
        for (uint8_t col = 0; col < 128; col++) {
            OLED_WR_Data(oled_compressed_buf[page * 128 + col]);
        }
    }

    // 通知LVGL刷新完成
    lv_disp_flush_ready(drv);
}

void lvgl_port_init(void)
{
    lv_init();

    // 初始化显示缓冲区：像素个数 = 128 * 32
    lv_disp_draw_buf_init(&draw_buf, buf_1, buf_2, 128 * 32);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 128;                 // 水平分辨率
    disp_drv.ver_res = 32;                  // 垂直分辨率
    disp_drv.flush_cb = ssd1306_flush_cb;   // 刷新回调
    disp_drv.draw_buf = &draw_buf;          // 显示缓冲区
    disp_drv.full_refresh = 1;              // 启用全屏刷新模式

    lv_disp_drv_register(&disp_drv);
}

void lvgl_task_handler(void)
{
    lv_timer_handler();
}
