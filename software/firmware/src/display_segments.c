#include "display_segments.h"

#include "display.h"
#include "u8g2.h"

typedef enum {
    seg_a = 0x01,
    seg_b = 0x02,
    seg_c = 0x04,
    seg_d = 0x08,
    seg_e = 0x10,
    seg_f = 0x20,
    seg_g = 0x40
} display_seg_t;

typedef void (*display_draw_segment_func)(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, display_seg_t segments);

static void display_draw_digit_impl(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, uint8_t digit,
    display_draw_segment_func draw_func);
static void display_draw_msegment(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, display_seg_t segments);

void display_draw_mdigit(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, uint8_t digit)
{
    display_draw_digit_impl(u8g2, x, y, digit, display_draw_msegment);
}

void display_draw_digit_impl(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, uint8_t digit,
    display_draw_segment_func draw_func)
{
    display_seg_t segments = 0;
    switch(digit) {
    case 0:
        segments = seg_a | seg_b | seg_c | seg_d | seg_e | seg_f;
        break;
    case 1:
        segments = seg_b | seg_c;
        break;
    case 2:
        segments = seg_a | seg_b | seg_d | seg_e | seg_g;
        break;
    case 3:
        segments = seg_a | seg_b | seg_c | seg_d | seg_g;
        break;
    case 4:
        segments = seg_b | seg_c | seg_f | seg_g;
        break;
    case 5:
        segments = seg_a | seg_c | seg_d | seg_f | seg_g;
        break;
    case 6:
        segments = seg_a | seg_c | seg_d | seg_e | seg_f | seg_g;
        break;
    case 7:
        segments = seg_a | seg_b | seg_c;
        break;
    case 8:
        segments = seg_a | seg_b | seg_c | seg_d | seg_e | seg_f | seg_g;
        break;
    case 9:
        segments = seg_a | seg_b | seg_c | seg_d | seg_f | seg_g;
        break;
    default:
        break;
    }
    draw_func(u8g2, x, y, segments);
}

/**
 * Draw a segments of a digit on a 18x37 pixel grid
 */
void display_draw_msegment(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, display_seg_t segments)
{
    if ((segments & seg_a) != 0) {
        u8g2_DrawLine(u8g2, x + 1, y + 0, x + 16, y + 0);
        u8g2_DrawLine(u8g2, x + 2, y + 1, x + 15, y + 1);
        u8g2_DrawLine(u8g2, x + 3, y + 2, x + 14, y + 2);
    }
    if ((segments & seg_b) != 0) {
        u8g2_DrawLine(u8g2, x + 15, y + 3, x + 15, y + 16);
        u8g2_DrawLine(u8g2, x + 16, y + 2, x + 16, y + 17);
        u8g2_DrawLine(u8g2, x + 17, y + 1, x + 17, y + 16);
    }
    if ((segments & seg_c) != 0) {
        u8g2_DrawLine(u8g2, x + 15, y + 20, x + 15, y + 33);
        u8g2_DrawLine(u8g2, x + 16, y + 19, x + 16, y + 34);
        u8g2_DrawLine(u8g2, x + 17, y + 20, x + 17, y + 35);
    }
    if ((segments & seg_d) != 0) {
        u8g2_DrawLine(u8g2, x + 3, y + 34, x + 14, y + 34);
        u8g2_DrawLine(u8g2, x + 2, y + 35, x + 15, y + 35);
        u8g2_DrawLine(u8g2, x + 1, y + 36, x + 16, y + 36);
    }
    if ((segments & seg_e) != 0) {
        u8g2_DrawLine(u8g2, x + 0, y + 20, x + 0, y + 35);
        u8g2_DrawLine(u8g2, x + 1, y + 19, x + 1, y + 34);
        u8g2_DrawLine(u8g2, x + 2, y + 20, x + 2, y + 33);
    }
    if ((segments & seg_f) != 0) {
        u8g2_DrawLine(u8g2, x + 0, y + 1, x + 0, y + 16);
        u8g2_DrawLine(u8g2, x + 1, y + 2, x + 1, y + 17);
        u8g2_DrawLine(u8g2, x + 2, y + 3, x + 2, y + 16);
    }
    if ((segments & seg_g) != 0) {
        u8g2_DrawLine(u8g2, x + 3, y + 17, x + 14, y + 17);
        u8g2_DrawLine(u8g2, x + 2, y + 18, x + 15, y + 18);
        u8g2_DrawLine(u8g2, x + 3, y + 19, x + 14, y + 19);
    }
}
