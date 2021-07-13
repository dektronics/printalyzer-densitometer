#include "display_assets.h"

#include <string.h>

#define asset_reflection_40_width 40
#define asset_reflection_40_height 40
static unsigned char asset_reflection_40_bits[] = {
    0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00,
    0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0xf0, 0xff, 0x0f, 0x00, 0x00,
    0xf8, 0xff, 0x1f, 0x00, 0x00, 0xfe, 0x3c, 0x7f, 0x00, 0x00, 0x3f, 0x3c,
    0xfc, 0x00, 0x00, 0x0f, 0x3c, 0xf0, 0x00, 0x80, 0x07, 0x3c, 0xe0, 0x01,
    0xc0, 0x03, 0x00, 0xc0, 0x03, 0xc0, 0x03, 0x00, 0xc0, 0x03, 0xe0, 0x01,
    0x00, 0x80, 0x07, 0xe0, 0x01, 0x00, 0x80, 0x07, 0xe0, 0x00, 0x00, 0x00,
    0x07, 0xe0, 0x00, 0x00, 0x00, 0x07, 0xff, 0x0f, 0x18, 0xf0, 0xff, 0xff,
    0x0f, 0x3c, 0xf0, 0xff, 0xff, 0x0f, 0x3c, 0xf0, 0xff, 0xff, 0x0f, 0x18,
    0xf0, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x07,
    0xe0, 0x01, 0x00, 0x80, 0x07, 0xe0, 0x01, 0x00, 0x80, 0x07, 0xc0, 0x03,
    0x00, 0xc0, 0x03, 0xc0, 0x03, 0x00, 0xc0, 0x03, 0x80, 0x07, 0x3c, 0xe0,
    0x01, 0x00, 0x0f, 0x3c, 0xf0, 0x00, 0x00, 0x3f, 0x3c, 0xfc, 0x00, 0x00,
    0xfe, 0x3c, 0x7f, 0x00, 0x00, 0xf8, 0xff, 0x1f, 0x00, 0x00, 0xf0, 0xff,
    0x0f, 0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00,
    0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00 };

#define asset_reflection_40_1_width 40
#define asset_reflection_40_1_height 40
static unsigned char asset_reflection_40_1_bits[] = {
    0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00,
    0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0xf0, 0xff, 0x0f, 0x00, 0x00,
    0xf8, 0xff, 0x1f, 0x00, 0x00, 0xfe, 0x3c, 0x7f, 0x00, 0x00, 0x3f, 0x3c,
    0xfc, 0x00, 0x00, 0x0f, 0x3c, 0xf0, 0x00, 0x80, 0x07, 0x00, 0xe0, 0x01,
    0xc0, 0x03, 0x00, 0xc0, 0x03, 0xc0, 0x03, 0x00, 0xc0, 0x03, 0xe0, 0x01,
    0x00, 0x80, 0x07, 0xe0, 0x01, 0x00, 0x80, 0x07, 0xe0, 0x00, 0x18, 0x00,
    0x07, 0xe0, 0x00, 0x18, 0x00, 0x07, 0xff, 0x07, 0x3c, 0xe0, 0xff, 0xff,
    0x07, 0xff, 0xe0, 0xff, 0xff, 0x07, 0xff, 0xe0, 0xff, 0xff, 0x07, 0x3c,
    0xe0, 0xff, 0xe0, 0x00, 0x18, 0x00, 0x07, 0xe0, 0x00, 0x18, 0x00, 0x07,
    0xe0, 0x01, 0x00, 0x80, 0x07, 0xe0, 0x01, 0x00, 0x80, 0x07, 0xc0, 0x03,
    0x00, 0xc0, 0x03, 0xc0, 0x03, 0x00, 0xc0, 0x03, 0x80, 0x07, 0x00, 0xe0,
    0x01, 0x00, 0x0f, 0x3c, 0xf0, 0x00, 0x00, 0x3f, 0x3c, 0xfc, 0x00, 0x00,
    0xfe, 0x3c, 0x7f, 0x00, 0x00, 0xf8, 0xff, 0x1f, 0x00, 0x00, 0xf0, 0xff,
    0x0f, 0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00,
    0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00 };

#define asset_reflection_40_2_width 40
#define asset_reflection_40_2_height 40
static unsigned char asset_reflection_40_2_bits[] = {
    0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00,
    0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0xf0, 0xff, 0x0f, 0x00, 0x00,
    0xf8, 0xff, 0x1f, 0x00, 0x00, 0xfe, 0x3c, 0x7f, 0x00, 0x00, 0x3f, 0x3c,
    0xfc, 0x00, 0x00, 0x0f, 0x00, 0xf0, 0x00, 0x80, 0x07, 0x00, 0xe0, 0x01,
    0xc0, 0x03, 0x00, 0xc0, 0x03, 0xc0, 0x03, 0x00, 0xc0, 0x03, 0xe0, 0x01,
    0x18, 0x80, 0x07, 0xe0, 0x01, 0x18, 0x80, 0x07, 0xe0, 0x00, 0x3c, 0x00,
    0x07, 0xe0, 0x00, 0x7e, 0x00, 0x07, 0xff, 0x03, 0xff, 0xc0, 0xff, 0xff,
    0xc3, 0xff, 0xc3, 0xff, 0xff, 0xc3, 0xff, 0xc3, 0xff, 0xff, 0x03, 0xff,
    0xc0, 0xff, 0xe0, 0x00, 0x7e, 0x00, 0x07, 0xe0, 0x00, 0x3c, 0x00, 0x07,
    0xe0, 0x01, 0x18, 0x80, 0x07, 0xe0, 0x01, 0x18, 0x80, 0x07, 0xc0, 0x03,
    0x00, 0xc0, 0x03, 0xc0, 0x03, 0x00, 0xc0, 0x03, 0x80, 0x07, 0x00, 0xe0,
    0x01, 0x00, 0x0f, 0x00, 0xf0, 0x00, 0x00, 0x3f, 0x3c, 0xfc, 0x00, 0x00,
    0xfe, 0x3c, 0x7f, 0x00, 0x00, 0xf8, 0xff, 0x1f, 0x00, 0x00, 0xf0, 0xff,
    0x0f, 0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00,
    0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00 };

#define asset_transmission_40_width 40
#define asset_transmission_40_height 40
static unsigned char asset_transmission_40_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0xf0, 0xff, 0x0f, 0x00, 0x00,
    0xf8, 0xff, 0x1f, 0x00, 0x00, 0xfe, 0x00, 0x7f, 0x00, 0x00, 0x3f, 0x00,
    0xfc, 0x00, 0x00, 0x0f, 0x00, 0xf0, 0x00, 0x80, 0x07, 0x00, 0xe0, 0x01,
    0xc0, 0x03, 0x00, 0xc0, 0x03, 0xc0, 0x03, 0x18, 0xc0, 0x03, 0xe0, 0x01,
    0x18, 0x80, 0x07, 0xe0, 0x01, 0x3c, 0x80, 0x07, 0xe0, 0x00, 0x3c, 0x00,
    0x07, 0xe0, 0x00, 0x7e, 0x00, 0x07, 0xe0, 0x80, 0xff, 0x01, 0x07, 0xe0,
    0xe0, 0xff, 0x07, 0x07, 0xe0, 0xe0, 0xff, 0x07, 0x07, 0xe0, 0x80, 0xff,
    0x01, 0x07, 0xe0, 0x00, 0x7e, 0x00, 0x07, 0xe0, 0x00, 0x3c, 0x00, 0x07,
    0xe0, 0x01, 0x3c, 0x80, 0x07, 0xe0, 0x01, 0x18, 0x80, 0x07, 0xc0, 0x03,
    0x18, 0xc0, 0x03, 0xc0, 0x03, 0x00, 0xc0, 0x03, 0x80, 0x07, 0x00, 0xe0,
    0x01, 0x00, 0x0f, 0x00, 0xf0, 0x00, 0x00, 0x3f, 0x00, 0xfc, 0x00, 0x00,
    0xfe, 0x00, 0x7f, 0x00, 0x00, 0xf8, 0xff, 0x1f, 0x00, 0x00, 0xf0, 0xff,
    0x0f, 0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define asset_transmission_40_1_width 40
#define asset_transmission_40_1_height 40
static unsigned char asset_transmission_40_1_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0xf0, 0xff, 0x0f, 0x00, 0x00,
    0xf8, 0xff, 0x1f, 0x00, 0x00, 0xfe, 0x00, 0x7f, 0x00, 0x00, 0x3f, 0x00,
    0xfc, 0x00, 0x00, 0x0f, 0x00, 0xf0, 0x00, 0x80, 0x07, 0x18, 0xe0, 0x01,
    0xc0, 0x03, 0x18, 0xc0, 0x03, 0xc0, 0x03, 0x3c, 0xc0, 0x03, 0xe0, 0x01,
    0x3c, 0x80, 0x07, 0xe0, 0x01, 0x7e, 0x80, 0x07, 0xe0, 0x00, 0xff, 0x00,
    0x07, 0xe0, 0x80, 0xff, 0x01, 0x07, 0xe0, 0xe0, 0xff, 0x07, 0x07, 0xe0,
    0xf8, 0xff, 0x1f, 0x07, 0xe0, 0xf8, 0xff, 0x1f, 0x07, 0xe0, 0xe0, 0xff,
    0x07, 0x07, 0xe0, 0x80, 0xff, 0x01, 0x07, 0xe0, 0x00, 0xff, 0x00, 0x07,
    0xe0, 0x01, 0x7e, 0x80, 0x07, 0xe0, 0x01, 0x3c, 0x80, 0x07, 0xc0, 0x03,
    0x3c, 0xc0, 0x03, 0xc0, 0x03, 0x18, 0xc0, 0x03, 0x80, 0x07, 0x18, 0xe0,
    0x01, 0x00, 0x0f, 0x00, 0xf0, 0x00, 0x00, 0x3f, 0x00, 0xfc, 0x00, 0x00,
    0xfe, 0x00, 0x7f, 0x00, 0x00, 0xf8, 0xff, 0x1f, 0x00, 0x00, 0xf0, 0xff,
    0x0f, 0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define asset_transmission_40_2_width 40
#define asset_transmission_40_2_height 40
static unsigned char asset_transmission_40_2_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0xf0, 0xff, 0x0f, 0x00, 0x00,
    0xf8, 0xff, 0x1f, 0x00, 0x00, 0xfe, 0x00, 0x7f, 0x00, 0x00, 0x3f, 0x18,
    0xfc, 0x00, 0x00, 0x0f, 0x18, 0xf0, 0x00, 0x80, 0x07, 0x3c, 0xe0, 0x01,
    0xc0, 0x03, 0x3c, 0xc0, 0x03, 0xc0, 0x03, 0x7e, 0xc0, 0x03, 0xe0, 0x01,
    0x7e, 0x80, 0x07, 0xe0, 0x01, 0xff, 0x80, 0x07, 0xe0, 0x80, 0xff, 0x01,
    0x07, 0xe0, 0xe0, 0xff, 0x07, 0x07, 0xe0, 0xf8, 0xff, 0x1f, 0x07, 0xe0,
    0xfe, 0xff, 0x7f, 0x07, 0xe0, 0xfe, 0xff, 0x7f, 0x07, 0xe0, 0xf8, 0xff,
    0x1f, 0x07, 0xe0, 0xe0, 0xff, 0x07, 0x07, 0xe0, 0x80, 0xff, 0x01, 0x07,
    0xe0, 0x01, 0xff, 0x80, 0x07, 0xe0, 0x01, 0x7e, 0x80, 0x07, 0xc0, 0x03,
    0x7e, 0xc0, 0x03, 0xc0, 0x03, 0x3c, 0xc0, 0x03, 0x80, 0x07, 0x3c, 0xe0,
    0x01, 0x00, 0x0f, 0x18, 0xf0, 0x00, 0x00, 0x3f, 0x18, 0xfc, 0x00, 0x00,
    0xfe, 0x00, 0x7f, 0x00, 0x00, 0xf8, 0xff, 0x1f, 0x00, 0x00, 0xf0, 0xff,
    0x0f, 0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define ASSET_ENTRY(x) { x##_bits, x##_width, x##_height }

static asset_info_t asset_list[] = {
    ASSET_ENTRY(asset_reflection_40),
    ASSET_ENTRY(asset_reflection_40_1),
    ASSET_ENTRY(asset_reflection_40_2),
    ASSET_ENTRY(asset_transmission_40),
    ASSET_ENTRY(asset_transmission_40_1),
    ASSET_ENTRY(asset_transmission_40_2)
};

uint8_t display_asset_get(asset_info_t *asset_info, asset_name_t asset_name)
{
    if (asset_info && asset_name >= 0 && asset_name < ASSET_MAX) {
        memcpy(asset_info, &asset_list[asset_name], sizeof(asset_info_t));
        return 1;
    } else {
        return 0;
    }
}
