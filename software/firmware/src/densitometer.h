#ifndef DENSITOMETER_H
#define DENSITOMETER_H

#include <math.h>

#include "sensor.h"

typedef enum {
    DENSITOMETER_OK = 0,
    DENSITOMETER_CAL_ERROR,
    DENSITOMETER_SENSOR_ERROR
} densitometer_result_t;

densitometer_result_t densitometer_reflection_measure(sensor_read_callback_t callback, void *user_data);
float densitometer_reflection_get_last_reading();

densitometer_result_t densitometer_transmission_measure(sensor_read_callback_t callback, void *user_data);
float densitometer_transmission_get_last_reading();

void densitometer_calibrate_gain();
densitometer_result_t densitometer_calibrate_reflection_lo(float cal_lo_d, sensor_read_callback_t callback, void *user_data);
densitometer_result_t densitometer_calibrate_reflection_hi(float cal_hi_d, sensor_read_callback_t callback, void *user_data);
densitometer_result_t densitometer_calibrate_transmission_zero(sensor_read_callback_t callback, void *user_data);
densitometer_result_t densitometer_calibrate_transmission_hi(float cal_hi_d, sensor_read_callback_t callback, void *user_data);

#endif /* DENSITOMETER_H */
