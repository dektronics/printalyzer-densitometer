/*
 * TSL2591 - Light-to-Digital Converter with I2C Interface
 */

#ifndef TSL2591_H
#define TSL2591_H

#include <stdbool.h>

#include "stm32l0xx_hal.h"

typedef enum {
    TSL2591_GAIN_LOW = 0,
    TSL2591_GAIN_MEDIUM = 1,
    TSL2591_GAIN_HIGH = 2,
    TSL2591_GAIN_MAXIMUM = 3
} tsl2591_gain_t;

/* Gain value ranges, as specified by the datasheet */
#define TSL2591_GAIN_MEDIUM_MIN      22
#define TSL2591_GAIN_MEDIUM_TYP      24.5F
#define TSL2591_GAIN_MEDIUM_MAX      27

#define TSL2591_GAIN_HIGH_MIN        360
#define TSL2591_GAIN_HIGH_TYP        400
#define TSL2591_GAIN_HIGH_MAX        440

#define TSL2591_GAIN_MAXIMUM_CH0_MIN 8500
#define TSL2591_GAIN_MAXIMUM_CH0_TYP 9200
#define TSL2591_GAIN_MAXIMUM_CH0_MAX 9900

#define TSL2591_GAIN_MAXIMUM_CH1_MIN 9100
#define TSL2591_GAIN_MAXIMUM_CH1_TYP 9900
#define TSL2591_GAIN_MAXIMUM_CH1_MAX 10700

typedef enum {
    TSL2591_TIME_100MS = 0,
    TSL2591_TIME_200MS = 1,
    TSL2591_TIME_300MS = 2,
    TSL2591_TIME_400MS = 3,
    TSL2591_TIME_500MS = 4,
    TSL2591_TIME_600MS = 5
} tsl2591_time_t;

typedef enum {
    TSL2591_PERSIST_EVERY = 0, /*!< Every ALS cycle generates an interrupt */
    TSL2591_PERSIST_ANY   = 1, /*!< Any value outside of threshold range */
    TSL2591_PERSIST_2     = 2,
    TSL2591_PERSIST_3     = 3,
    TSL2591_PERSIST_5     = 4,
    TSL2591_PERSIST_10    = 5,
    TSL2591_PERSIST_15    = 6,
    TSL2591_PERSIST_20    = 7,
    TSL2591_PERSIST_25    = 8,
    TSL2591_PERSIST_30    = 9,
    TSL2591_PERSIST_35    = 10,
    TSL2591_PERSIST_40    = 11,
    TSL2591_PERSIST_45    = 12,
    TSL2591_PERSIST_50    = 13,
    TSL2591_PERSIST_55    = 14,
    TSL2591_PERSIST_60    = 15
} tsl2591_persist_t;

/* Enable Register Values */
#define TSL2591_ENABLE_NPIEN 0x80 /*!< No persist interrupt enable */
#define TSL2591_ENABLE_SAI   0x40 /*!< Sleep after interrupt */
#define TSL2591_ENABLE_AIEN  0x10 /*!< ALS interrupt enable */
#define TSL2591_ENABLE_AEN   0x02 /*!< ALS enable */
#define TSL2591_ENABLE_PON   0x01 /*!< Power ON */

/* Status Register Values */
#define TSL2591_STATUS_NPINTR 0x20 /*!< No-persist interrupt */
#define TSL2591_STATUS_AINT   0x10 /*!< ALS interrupt */
#define TSL2591_STATUS_AVALID 0x01 /*!< ALS valid */

HAL_StatusTypeDef tsl2591_init(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef tsl2591_set_enable(I2C_HandleTypeDef *hi2c, uint8_t value);
HAL_StatusTypeDef tsl2591_enable(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef tsl2591_disable(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef tsl2591_clear_als_int(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef tsl2591_set_config(I2C_HandleTypeDef *hi2c, tsl2591_gain_t gain, tsl2591_time_t time);
HAL_StatusTypeDef tsl2591_get_config(I2C_HandleTypeDef *hi2c, tsl2591_gain_t *gain, tsl2591_time_t *time);

HAL_StatusTypeDef tsl2591_set_als_low_int_threshold(I2C_HandleTypeDef *hi2c, uint16_t value);
HAL_StatusTypeDef tsl2591_set_als_high_int_threshold(I2C_HandleTypeDef *hi2c, uint16_t value);

HAL_StatusTypeDef tsl2591_set_persist(I2C_HandleTypeDef *hi2c, tsl2591_persist_t value);

HAL_StatusTypeDef tsl2591_get_status(I2C_HandleTypeDef *hi2c, uint8_t *value);
HAL_StatusTypeDef tsl2591_get_status_valid(I2C_HandleTypeDef *hi2c, bool *valid);

HAL_StatusTypeDef tsl2591_get_full_channel_data(I2C_HandleTypeDef *hi2c, uint16_t *ch0_val, uint16_t *ch1_val);

#endif /* TSL2591_H */
