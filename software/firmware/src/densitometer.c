#include "densitometer.h"

#define LOG_TAG "densitometer"

#include <math.h>
#include <elog.h>
#include <printf.h>

#include "settings.h"
#include "sensor.h"
#include "task_sensor.h"
#include "light.h"
#include "cdc_handler.h"
#include "util.h"

float densitometer_reflection_d = NAN;
float densitometer_reflection_zero_d = NAN;
float densitometer_transmission_d = NAN;
float densitometer_transmission_zero_d = NAN;

densitometer_result_t densitometer_reflection_measure(sensor_read_callback_t callback, void *user_data)
{
    float cal_lo_d;
    float cal_lo_value;
    float cal_hi_d;
    float cal_hi_value;

    /* Get the current calibration values */
    settings_get_cal_reflection_lo(&cal_lo_d, &cal_lo_value);
    settings_get_cal_reflection_hi(&cal_hi_d, &cal_hi_value);

    /* Check that the calibration values are usable */
    if (cal_lo_d < 0.0F || cal_hi_d <= cal_lo_d
        || cal_lo_value < 0.0F || cal_hi_value >= cal_lo_value) {

        log_w("Invalid calibration values");
        log_w("CAL-LO: D=%.2f, VALUE=%f", cal_lo_d, cal_lo_value);
        log_w("CAL-HI: D=%.2f, VALUE=%f", cal_hi_d, cal_hi_value);

        return DENSITOMETER_CAL_ERROR;
    }

    /* Perform sensor read */
    float ch0_basic;
    float ch1_basic;
    if (sensor_read_target(SENSOR_LIGHT_REFLECTION, &ch0_basic, &ch1_basic, callback, user_data) != osOK) {
        log_w("Sensor read error");
        sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, LIGHT_REFLECTION_IDLE);
        return DENSITOMETER_SENSOR_ERROR;
    }

    /* Make sure the two channels don't overlap the wrong way */
    if (ch1_basic >= ch0_basic) { ch1_basic = 0; }

    /* Combine and correct the basic reading */
    float meas_value = ch0_basic - ch1_basic;
    float corr_value = sensor_apply_slope_calibration(meas_value);

    /* Convert all values into log units */
    float meas_ll = log10f(corr_value);
    float cal_hi_ll = log10f(cal_hi_value);
    float cal_lo_ll = log10f(cal_lo_value);

    /* Calculate the slope of the line */
    float m = (cal_hi_d - cal_lo_d) / (cal_hi_ll - cal_lo_ll);

    /* Calculate the measured density */
    float meas_d = (m * (meas_ll - cal_lo_ll)) + cal_lo_d;

    log_i("D=%.2f, VALUE=%f,%f", meas_d, meas_value, corr_value);

    /* Clamp the return value to be within an acceptable range */
    if (meas_d < 0.0F) { meas_d = 0.0F; }
    else if (meas_d > 2.5F) { meas_d = 2.5F; }

    densitometer_reflection_d = meas_d;

    /* Set light back to idle */
    sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, LIGHT_REFLECTION_IDLE);

    cdc_send_density_reading('R', meas_d, meas_value, corr_value);

    return DENSITOMETER_OK;
}

void densitometer_reflection_set_zero()
{
    if (!isnanf(densitometer_reflection_d)) {
        densitometer_reflection_zero_d = densitometer_reflection_d;
    }
}

void densitometer_reflection_clear_zero()
{
    densitometer_reflection_zero_d = NAN;
}

bool densitometer_reflection_has_zero()
{
    return !isnanf(densitometer_reflection_zero_d);
}

float densitometer_reflection_get_last_reading()
{
    if (!isnanf(densitometer_reflection_zero_d)) {
        return densitometer_reflection_d - densitometer_reflection_zero_d;
    } else {
        return densitometer_reflection_d;
    }
}

densitometer_result_t densitometer_transmission_measure(sensor_read_callback_t callback, void *user_data)
{
    float cal_zero_value;
    float cal_hi_d;
    float cal_hi_value;

    /* Get the current calibration values */
    settings_get_cal_transmission_zero(&cal_zero_value);
    settings_get_cal_transmission_hi(&cal_hi_d, &cal_hi_value);

    /* Check that the calibration values are usable */
    if (cal_zero_value <= 0.0F || cal_hi_d <= 0.0F || cal_hi_value <= 0.0F
        || cal_hi_value >= cal_zero_value) {

        log_w("Invalid calibration values");
        log_w("CAL-ZERO: VALUE=%f", cal_zero_value);
        log_w("CAL-HI: D=%.2f, VALUE=%f", cal_hi_d, cal_hi_value);

        return DENSITOMETER_CAL_ERROR;
    }

    /* Perform sensor read */
    float ch0_basic;
    float ch1_basic;
    if (sensor_read_target(SENSOR_LIGHT_TRANSMISSION, &ch0_basic, &ch1_basic, callback, user_data) != osOK) {
        log_w("Sensor read error");
        sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, LIGHT_TRANSMISSION_IDLE);
        return DENSITOMETER_SENSOR_ERROR;
    }

    if (ch1_basic >= ch0_basic) { ch1_basic = 0; }

    /* Combine and correct the basic reading */
    float meas_value = ch0_basic - ch1_basic;
    float corr_value = sensor_apply_slope_calibration(meas_value);

    /* Calculate the measured CAL-HI density relative to the zero value */
    float cal_hi_meas_d = -1.0F * log10f(cal_hi_value / cal_zero_value);

    /* Calculate the measured target density relative to the zero value */
    float meas_d = -1.0F * log10f(corr_value / cal_zero_value);

    /* Calculate the adjustment factor */
    float adj_factor = cal_hi_d / cal_hi_meas_d;

    /* Calculate the calibration corrected density */
    float corr_d = meas_d * adj_factor;

    log_i("D=%.2f, VALUE=%f,%f", corr_d, meas_value, corr_value);

    /* Clamp the return value to be within an acceptable range */
    if (corr_d < 0.0F) { corr_d = 0.0F; }
    else if (corr_d > TRANSMISSION_MAX_D) { corr_d = TRANSMISSION_MAX_D; }

    densitometer_transmission_d = corr_d;

    /* Set light back to idle */
    sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, LIGHT_TRANSMISSION_IDLE);

    cdc_send_density_reading('T', meas_d, meas_value, corr_value);

    return DENSITOMETER_OK;
}

void densitometer_transmission_set_zero()
{
    if (!isnanf(densitometer_transmission_d)) {
        densitometer_transmission_zero_d = densitometer_transmission_d;
    }
}

void densitometer_transmission_clear_zero()
{
    densitometer_transmission_zero_d = NAN;
}

bool densitometer_transmission_has_zero()
{
    return !isnanf(densitometer_transmission_zero_d);
}

float densitometer_transmission_get_last_reading()
{
    if (!isnanf(densitometer_transmission_zero_d)) {
        return densitometer_transmission_d - densitometer_transmission_zero_d;
    } else {
        return densitometer_transmission_d;
    }
}

densitometer_result_t densitometer_calibrate_reflection_lo(float cal_lo_d, sensor_read_callback_t callback, void *user_data)
{
    float ch0_basic;
    float ch1_basic;

    /* Make sure the argument is within a reasonable range */
    if (cal_lo_d < 0.00F || cal_lo_d > REFLECTION_MAX_D) {
        return DENSITOMETER_CAL_ERROR;
    }

    /* Perform sensor read */
    if (sensor_read_target(SENSOR_LIGHT_REFLECTION, &ch0_basic, &ch1_basic, callback, user_data) != osOK) {
        log_w("Sensor read error");
        sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, LIGHT_REFLECTION_IDLE);
        return DENSITOMETER_SENSOR_ERROR;
    }

    if (ch1_basic >= ch0_basic) { ch1_basic = 0; }

    /* Combine and correct the basic reading */
    float meas_value = ch0_basic - ch1_basic;
    float corr_value = sensor_apply_slope_calibration(meas_value);

    if (meas_value < 0.01F || corr_value < 0.01F) {
        return DENSITOMETER_CAL_ERROR;
    }

    /* Save the calibration value */
    settings_set_cal_reflection_lo(cal_lo_d, corr_value);

    /* Set light back to idle */
    sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, LIGHT_REFLECTION_IDLE);

    return DENSITOMETER_OK;
}

densitometer_result_t densitometer_calibrate_reflection_hi(float cal_hi_d, sensor_read_callback_t callback, void *user_data)
{
    float ch0_basic;
    float ch1_basic;

    /* Make sure the argument is within a reasonable range */
    if (cal_hi_d < 0.00F || cal_hi_d > REFLECTION_MAX_D) {
        return DENSITOMETER_CAL_ERROR;
    }

    /* Perform sensor read */
    if (sensor_read_target(SENSOR_LIGHT_REFLECTION, &ch0_basic, &ch1_basic, callback, user_data) != osOK) {
        log_w("Sensor read error");
        sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, LIGHT_REFLECTION_IDLE);
        return DENSITOMETER_SENSOR_ERROR;
    }

    if (ch1_basic >= ch0_basic) { ch1_basic = 0; }

    /* Combine and correct the basic reading */
    float meas_value = ch0_basic - ch1_basic;
    float corr_value = sensor_apply_slope_calibration(meas_value);

    if (meas_value < 0.01F || corr_value < 0.01F) {
        return DENSITOMETER_CAL_ERROR;
    }

    /* Save the calibration value */
    settings_set_cal_reflection_hi(cal_hi_d, corr_value);

    /* Set light back to idle */
    sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, LIGHT_REFLECTION_IDLE);

    return DENSITOMETER_OK;
}

densitometer_result_t densitometer_calibrate_transmission_zero(sensor_read_callback_t callback, void *user_data)
{
    float ch0_basic;
    float ch1_basic;

    /* Perform sensor read */
    if (sensor_read_target(SENSOR_LIGHT_TRANSMISSION, &ch0_basic, &ch1_basic, callback, user_data) != osOK) {
        log_w("Sensor read error");
        sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, LIGHT_TRANSMISSION_IDLE);
        return DENSITOMETER_SENSOR_ERROR;
    }

    if (ch1_basic >= ch0_basic) { ch1_basic = 0; }

    /* Combine and correct the basic reading */
    float meas_value = ch0_basic - ch1_basic;
    float corr_value = sensor_apply_slope_calibration(meas_value);

    if (meas_value < 0.01F || corr_value < 0.01F) {
        return DENSITOMETER_CAL_ERROR;
    }

    /* Save the calibration value */
    settings_set_cal_transmission_zero(corr_value);

    /* Set light back to idle */
    sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, LIGHT_TRANSMISSION_IDLE);

    return DENSITOMETER_OK;
}

densitometer_result_t densitometer_calibrate_transmission_hi(float cal_hi_d, sensor_read_callback_t callback, void *user_data)
{
    float ch0_basic;
    float ch1_basic;

    /* Make sure the argument is within a reasonable range */
    if (cal_hi_d < 0.00F || cal_hi_d > TRANSMISSION_MAX_D) {
        return DENSITOMETER_CAL_ERROR;
    }

    /* Perform sensor read */
    if (sensor_read_target(SENSOR_LIGHT_TRANSMISSION, &ch0_basic, &ch1_basic, callback, user_data) != osOK) {
        log_w("Sensor read error");
        sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, LIGHT_TRANSMISSION_IDLE);
        return DENSITOMETER_SENSOR_ERROR;
    }

    if (ch1_basic >= ch0_basic) { ch1_basic = 0; }

    /* Combine and correct the basic reading */
    float meas_value = ch0_basic - ch1_basic;
    float corr_value = sensor_apply_slope_calibration(meas_value);

    if (meas_value < 0.01F || corr_value < 0.01F) {
        return DENSITOMETER_CAL_ERROR;
    }

    /* Save the calibration value */
    settings_set_cal_transmission_hi(cal_hi_d, corr_value);

    /* Set light back to idle */
    sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, LIGHT_TRANSMISSION_IDLE);

    return DENSITOMETER_OK;
}
