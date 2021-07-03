#ifndef __U8G2_STM32_HAL_H
#define __U8G2_STM32_HAL_H

#include "stm32l0xx_hal.h"
#include "u8g2.h"

void u8g2_stm32_hal_init(SPI_HandleTypeDef *hspi);
uint8_t u8g2_stm32_spi_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8g2_stm32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif /* __U8G2_STM32_HAL_H */
