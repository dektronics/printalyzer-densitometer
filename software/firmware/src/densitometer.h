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

densitometer_result_t densitometer_calibrate_reflection_lo(float cal_lo_d, sensor_read_callback_t callback, void *user_data);
densitometer_result_t densitometer_calibrate_reflection_hi(float cal_hi_d, sensor_read_callback_t callback, void *user_data);
densitometer_result_t densitometer_calibrate_transmission_zero(sensor_read_callback_t callback, void *user_data);
densitometer_result_t densitometer_calibrate_transmission_hi(float cal_hi_d, sensor_read_callback_t callback, void *user_data);

#endif /* DENSITOMETER_H */
