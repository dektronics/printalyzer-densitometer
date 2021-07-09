#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include "stm32l0xx_hal.h"

typedef enum {
    DISPLAY_MODE_REFLECTION,
    DISPLAY_MODE_TRANSMISSION
} display_mode_t;

typedef struct {
    display_mode_t mode;
    uint16_t density100;
} display_main_elements_t;

HAL_StatusTypeDef display_init(SPI_HandleTypeDef *hspi);

void display_clear();
void display_enable(bool enabled);
void display_set_contrast(uint8_t value);
uint8_t display_get_contrast();
void display_set_brightness(uint8_t value);
uint8_t display_get_brightness();
void display_draw_test_pattern(bool mode);
void display_static_list(const char *title, const char *list);

void display_draw_main_elements(const display_main_elements_t *elements);

#endif /* DISPLAY_H */
