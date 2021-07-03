#include "display.h"

#include "u8g2_stm32_hal.h"
#include "u8g2.h"

static u8g2_t u8g2;
static uint8_t display_contrast = 0x9F;
static uint8_t display_brightness = 0x0F;

/* Library function declarations */
void u8g2_DrawSelectionList(u8g2_t *u8g2, u8sl_t *u8sl, u8g2_uint_t y, const char *s);

static void display_set_freq(uint8_t value);

HAL_StatusTypeDef display_init(SPI_HandleTypeDef *hspi)
{
    /* Configure the SPI parameters for the STM32 HAL */
    u8g2_stm32_hal_init(hspi);

    /* Initialize the display driver */
    u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_R2, u8g2_stm32_spi_byte_cb, u8g2_stm32_gpio_and_delay_cb);

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    /*
     * Slightly increase the display refresh frequency
     * Oscillator Frequency (A[7:4])
     * Display Clock Divide Ratio (D)(A[3:0])
     * Default = 0x80 (div=1, Fosc=8)
     */
    display_set_freq(0xF0);

    return HAL_OK;
}

void display_set_freq(uint8_t value)
{
    /* This command sequence is specific to the SSD1306 */
    u8x8_t *u8x8 = &(u8g2.u8x8);
    u8x8_cad_StartTransfer(u8x8);
    u8x8_cad_SendCmd(u8x8, 0x0D5);
    u8x8_cad_SendArg(u8x8, value);
    u8x8_cad_EndTransfer(u8x8);
}

void display_clear()
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_SendBuffer(&u8g2);
}

void display_enable(bool enabled)
{
    u8g2_SetPowerSave(&u8g2, enabled ? 0 : 1);
}

void display_set_contrast(uint8_t value)
{
    u8g2_SetContrast(&u8g2, value);
    display_contrast = value;
}

uint8_t display_get_contrast()
{
    return display_contrast;
}

void display_set_brightness(uint8_t value)
{
    uint8_t arg = value & 0x0F;

    u8x8_t *u8x8 = &(u8g2.u8x8);
    u8x8_cad_StartTransfer(u8x8);
    u8x8_cad_SendCmd(u8x8, 0x0C7);
    u8x8_cad_SendArg(u8x8, arg);
    u8x8_cad_EndTransfer(u8x8);

    display_brightness = arg;
}

uint8_t display_get_brightness()
{
    return display_brightness;
}

void display_draw_test_pattern(bool mode)
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetDrawColor(&u8g2, 1);

    int h = u8g2_GetDisplayHeight(&u8g2);
    int w = u8g2_GetDisplayWidth(&u8g2);

    bool draw = mode;
    for (int y = 0; y < h; y += 16) {
        for (int x = 0; x < w; x += 16) {
            draw = !draw;
            if (draw) {
                u8g2_DrawBox(&u8g2, x, y, 16, 16);
            }
        }
        draw = !draw;
    }

    u8g2_SendBuffer(&u8g2);
}

static void display_prepare_menu_font()
{
    u8g2_SetFont(&u8g2, u8g2_font_pxplusibmcga_8f);
    u8g2_SetFontMode(&u8g2, 0);
    u8g2_SetDrawColor(&u8g2, 1);
}

void display_static_list(const char *title, const char *list)
{
    /*
     * Based off u8g2_UserInterfaceSelectionList() with changes to use
     * full frame buffer mode and to remove actual menu functionality.
     */

    display_prepare_menu_font();
    display_clear();

    u8sl_t u8sl;
    u8g2_uint_t yy;

    u8g2_uint_t line_height = u8g2_GetAscent(&u8g2) - u8g2_GetDescent(&u8g2) + 1;

    uint8_t title_lines = u8x8_GetStringLineCnt(title);
    uint8_t display_lines;

    if (title_lines > 0) {
        display_lines = (u8g2_GetDisplayHeight(&u8g2) - 3) / line_height;
        u8sl.visible = display_lines;
        u8sl.visible -= title_lines;
    }
    else {
        display_lines = u8g2_GetDisplayHeight(&u8g2) / line_height;
        u8sl.visible = display_lines;
    }

    u8sl.total = u8x8_GetStringLineCnt(list);
    u8sl.first_pos = 0;
    u8sl.current_pos = -1;

    u8g2_SetFontPosBaseline(&u8g2);

    yy = u8g2_GetAscent(&u8g2);
    if (title_lines > 0) {
        yy += u8g2_DrawUTF8Lines(&u8g2, 0, yy, u8g2_GetDisplayWidth(&u8g2), line_height, title);
        u8g2_DrawHLine(&u8g2, 0, yy - line_height - u8g2_GetDescent(&u8g2) + 1, u8g2_GetDisplayWidth(&u8g2));
        yy += 3;
    }
    u8g2_DrawSelectionList(&u8g2, &u8sl, yy, list);

    u8g2_SendBuffer(&u8g2);
}
