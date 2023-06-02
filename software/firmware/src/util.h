#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include <cmsis_os.h>
#include "stm32l0xx_hal.h"

#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

#define TIME_AFTER(a, b)  (((int32_t)(a) - (int32_t)(b)) > 0)

/**
 * Refresh the watchdog timer
 */
void watchdog_refresh();

/**
 * Slow down the watchdog
 * This should only be used in a few very specific situations, where the
 * alternative is to refresh it inside of a tight loop.
 */
void watchdog_slow();

/**
 * Return the watchdog to its normal window.
 */
void watchdog_normal();

osStatus_t hal_to_os_status(HAL_StatusTypeDef hal_status);
HAL_StatusTypeDef os_to_hal_status(osStatus_t os_status);

void copy_from_u32(uint8_t *buf, uint32_t val);
uint32_t copy_to_u32(const uint8_t *buf);
void copy_from_f32(uint8_t *buf, float val);
float copy_to_f32(const uint8_t *buf);

bool is_valid_number(float num);

void replace_first_char(char *str, char orig, char repl);
void replace_all_char(char *str, char orig, char repl);

#endif /* UTIL_H */
