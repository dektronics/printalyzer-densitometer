#ifndef SENSOR_H
#define SENSOR_H

#include <stdbool.h>

#include "stm32l0xx_hal.h"

typedef enum {
    SENSOR_GAIN_CALIBRATION_STATUS_INIT = 0,
    SENSOR_GAIN_CALIBRATION_STATUS_MEDIUM,
    SENSOR_GAIN_CALIBRATION_STATUS_HIGH,
    SENSOR_GAIN_CALIBRATION_STATUS_MAXIMUM,
    SENSOR_GAIN_CALIBRATION_STATUS_FAILED,
    SENSOR_GAIN_CALIBRATION_STATUS_DONE
} sensor_gain_calibration_status_t;

typedef void (*sensor_gain_calibration_callback_t)(sensor_gain_calibration_status_t status, void *user_data);

typedef void (*sensor_read_callback_t)(void *user_data);

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
 * @param callback Callback to monitor progress of the calibration
 * @return HAL_OK on success
 */
HAL_StatusTypeDef sensor_gain_calibration(sensor_gain_calibration_callback_t callback, void *user_data);

/**
 * Perform a reading with the sensor.
 *
 * @param iterations Number of read iterations to average across
 * @param ch0_result Channel 0 result, in basic counts
 * @param ch1_result Channel 1 result, in basic counts
 * @return HAL_OK on success
 */
HAL_StatusTypeDef sensor_read(uint8_t iterations, float *ch0_result, float *ch1_result, sensor_read_callback_t callback, void *user_data);

#endif /* SENSOR_H */
