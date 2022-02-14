#ifndef DENSITOMETER_H
#define DENSITOMETER_H

#include <stdbool.h>
#include <math.h>

#include "sensor.h"

#define REFLECTION_MAX_D (2.50F)
#define TRANSMISSION_MAX_D (5.00F)

typedef enum {
    DENSITOMETER_OK = 0,
    DENSITOMETER_CAL_ERROR,
    DENSITOMETER_SENSOR_ERROR
} densitometer_result_t;

densitometer_result_t densitometer_reflection_measure(sensor_read_callback_t callback, void *user_data);
void densitometer_reflection_set_zero();
void densitometer_reflection_clear_zero();
bool densitometer_reflection_has_zero();
float densitometer_reflection_get_last_reading();

densitometer_result_t densitometer_transmission_measure(sensor_read_callback_t callback, void *user_data);
void densitometer_transmission_set_zero();
void densitometer_transmission_clear_zero();
bool densitometer_transmission_has_zero();
float densitometer_transmission_get_last_reading();

/**
 * Measure a reflection calibration target.
 * This should be called to measure both the CAL-LO and CAL-HI targets to
 * populate the settings_cal_reflection_t structure.
 *
 * @param cal_value Adjusted raw measurement to save as a calibration value
 * @param callback Called periodically during the measurement loop
 * @return Result code for the measurement process
 */
densitometer_result_t densitometer_calibrate_reflection(float *cal_value, sensor_read_callback_t callback, void *user_data);

/**
 * Measure a transmission calibration target.
 * This should be called to measure both the ZERO and CAL-HI targets to
 * populate the settings_cal_transmission_t structure.
 *
 * @param cal_value Adjusted raw measurement to save as a calibration value
 * @param callback Called periodically during the measurement loop
 * @return Result code for the measurement process
 */
densitometer_result_t densitometer_calibrate_transmission(float *cal_value, sensor_read_callback_t callback, void *user_data);

#endif /* DENSITOMETER_H */
