#ifndef DISPLAY_SEGMENTS_H
#define DISPLAY_SEGMENTS_H

#include "u8g2.h"

/**
 * These functions support drawing 7-segment style numeric digits.
 */

/**
 * Draw a 18x37 pixel digit
 */
void display_draw_mdigit(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, uint8_t digit);

#endif /* DISPLAY_SEGMENTS_H */
