/*
 * Common interface to higher level densitometer functions.
 * This serves as a general-purpose front end to standardize the process of
 * going from a series of device operations to a displayable density result.
 */
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

typedef struct __densitometer_t densitometer_t;

/**
 * Set a global flag to allow uncalibrated measurements
 *
 * Uncalibrated measurements are typically used as part of raw data capture
 * used for the purposes of sensor calibration. Displayable density readings
 * require target calibration data to be available.
 */
void densitometer_set_allow_uncalibrated_measurements(bool allow);

/**
 * Get the densitometer instance for measuring reflection density
 */
densitometer_t *densitometer_reflection();

/**
 * Get the densitometer instance for measuring transmission density
 */
densitometer_t *densitometer_transmission();

/**
 * Measure a target material to get its density.
 *
 * @param callback Called periodically during the measurement loop
 * @return Result code for the measurement process
 */
densitometer_result_t densitometer_measure(densitometer_t *densitometer, sensor_read_callback_t callback, void *user_data);

/**
 * Measure a calibration target.
 *
 * For reflection, this should be called to measure both the CAL-LO and CAL-HI
 * targets to populate the settings_cal_reflection_t structure.
 *
 * For transmission, this should be called to measure both the ZERO and CAL-HI
 * targets to populate the settings_cal_transmission_t structure.
 *
 * @param cal_value Adjusted raw measurement to save as a calibration value
 * @param callback Called periodically during the measurement loop
 * @return Result code for the measurement process
 */
densitometer_result_t densitometer_calibrate(densitometer_t *densitometer, float *cal_value, sensor_read_callback_t callback, void *user_data);

/**
 * Control the idle light for the densitometer mode.
 *
 * @param enabled True to activate at idle brightness, false to deactivate
 */
void densitometer_set_idle_light(const densitometer_t *densitometer, bool enabled);

void densitometer_set_zero(densitometer_t *densitometer);
void densitometer_clear_zero(densitometer_t *densitometer);
bool densitometer_has_zero(const densitometer_t *densitometer);
float densitometer_get_last_reading(const densitometer_t *densitometer);

#endif /* DENSITOMETER_H */
