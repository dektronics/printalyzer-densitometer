#ifndef SENSOR_H
#define SENSOR_H

#include <stdbool.h>
#include <cmsis_os.h>

#include "stm32l0xx_hal.h"
#include "tsl2591.h"

typedef enum {
    SENSOR_GAIN_CALIBRATION_STATUS_INIT = 0,
    SENSOR_GAIN_CALIBRATION_STATUS_MEDIUM,
    SENSOR_GAIN_CALIBRATION_STATUS_HIGH,
    SENSOR_GAIN_CALIBRATION_STATUS_MAXIMUM,
    SENSOR_GAIN_CALIBRATION_STATUS_FAILED,
    SENSOR_GAIN_CALIBRATION_STATUS_DONE
} sensor_gain_calibration_status_t;

typedef struct {
    uint16_t ch0_val;
    uint16_t ch1_val;
    tsl2591_gain_t gain;
    tsl2591_time_t time;
} sensor_reading_t;

typedef void (*sensor_gain_calibration_callback_t)(sensor_gain_calibration_status_t status, void *user_data);
typedef void (*sensor_time_calibration_callback_t)(tsl2591_time_t time, void *user_data);
typedef void (*sensor_read_callback_t)(void *user_data);

void task_sensor_run(void *argument);

bool sensor_is_initialized();

void sensor_start();
void sensor_stop();
void sensor_set_config(tsl2591_gain_t gain, tsl2591_time_t time);
osStatus_t sensor_get_next_reading(sensor_reading_t *reading, uint32_t timeout);

void sensor_int_handler();

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
 * Run the sensor integration time calibration process.
 *
 * This function will turn on the transmission LED and keep the sensor at a
 * constant gain setting, while running it through the full list of available
 * integration time settings. The results will be saved to allow sensor data
 * calculations to be normalized across different integration times.
 *
 * @return HAL_OK on success
 */
HAL_StatusTypeDef sensor_time_calibration(sensor_time_calibration_callback_t callback, void *user_data);

/**
 * Perform a reading with the sensor.
 *
 * @param iterations Number of read iterations to average across
 * @param ch0_result Channel 0 result, in basic counts
 * @param ch1_result Channel 1 result, in basic counts
 * @return HAL_OK on success
 */
HAL_StatusTypeDef sensor_read(uint8_t iterations, float *ch0_result, float *ch1_result, sensor_read_callback_t callback, void *user_data);

/**
 * Check the sensor reading to see if the sensor is saturated.
 *
 * @param reading Reading to check
 * @return True if saturated, false otherwise
 */
bool sensor_is_reading_saturated(const sensor_reading_t *reading);

/**
 * Convert sensor readings from raw counts to basic counts.
 *
 * Basic counts are normalized based on the sensor gain, integration time,
 * and various system constants. This allows them to be compared across
 * multiple readings and different device settings. All actual light
 * calculations shall be performed in terms of basic counts.
 *
 * @param gain Gain used to take the readings
 * @param time Integration time used to take the readings
 * @param ch0_val Raw counts for channel 0
 * @param ch1_val Raw counts for channel 1
 * @param ch0_basic Basic count output for channel 0
 * @param ch1_basic Basic count output for channel 1
 */
void sensor_convert_to_basic_counts(tsl2591_gain_t gain, tsl2591_time_t time, float ch0_val, float ch1_val, float *ch0_basic, float *ch1_basic);

#endif /* SENSOR_H */
