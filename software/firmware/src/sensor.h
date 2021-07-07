#ifndef SENSOR_H
#define SENSOR_H

#include <stdbool.h>

#include "stm32l0xx_hal.h"

HAL_StatusTypeDef sensor_init(I2C_HandleTypeDef *hi2c);

bool sensor_is_initialized();

/**
 * Run the sensor gain calibration process.
 *
 * This function will run the sensor and transmission LED through a series of
 * measurements to determine the actual gain values that correspond to each
 * gain setting on the sensor. The results will be saved for use in future
 * sensor data calculations.
 *
 * TODO Add a mechanism for progress reporting and cancellation when this is hooked up to a UI
 *
 * @return HAL_OK on success
 */
HAL_StatusTypeDef sensor_gain_calibration();

/**
 * Perform a reading with the sensor.
 *
 * @param iterations Number of read iterations to average across
 * @param ch0_result Channel 0 result, in basic counts
 * @param ch1_result Channel 1 result, in basic counts
 * @return HAL_OK on success
 */
HAL_StatusTypeDef sensor_read(uint8_t iterations, float *ch0_result, float *ch1_result);

#endif /* SENSOR_H */
