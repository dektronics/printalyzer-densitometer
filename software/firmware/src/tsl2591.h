/*
 * TSL2591 - Light-to-Digital Converter with I2C Interface
 */

#ifndef TSL2591_H
#define TSL2591_H

#include <stdbool.h>

#include "stm32l0xx_hal.h"

/* Gain values from TSL2591 EVM software */
typedef enum {
    TSL2591_GAIN_LOW = 0,    /* 1x */
    TSL2591_GAIN_MEDIUM = 1, /* 24x */
    TSL2591_GAIN_HIGH = 2,   /* 384x */
    TSL2591_GAIN_MAXIMUM = 3 /* 9216x */
} tsl2591_gain_t;

typedef enum {
    TSL2591_TIME_100MS = 0,
    TSL2591_TIME_200MS = 1,
    TSL2591_TIME_300MS = 2,
    TSL2591_TIME_400MS = 3,
    TSL2591_TIME_500MS = 4,
    TSL2591_TIME_600MS = 5
} tsl2591_time_t;

HAL_StatusTypeDef tsl2591_init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef tsl2591_enable(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef tsl2591_disable(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef tsl2591_set_config(I2C_HandleTypeDef *hi2c, tsl2591_gain_t gain, tsl2591_time_t time);
HAL_StatusTypeDef tsl2591_get_config(I2C_HandleTypeDef *hi2c, tsl2591_gain_t *gain, tsl2591_time_t *time);

HAL_StatusTypeDef tsl2591_get_status_valid(I2C_HandleTypeDef *hi2c, bool *valid);

HAL_StatusTypeDef tsl2591_get_full_channel_data(I2C_HandleTypeDef *hi2c, uint16_t *ch0_val, uint16_t *ch1_val);

#endif /* TSL2591_H */
