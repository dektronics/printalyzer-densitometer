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
#include "hid_handler.h"
#include "util.h"

static densitometer_result_t reflection_measure(densitometer_t *densitometer, sensor_read_callback_t callback, void *user_data);
static densitometer_result_t transmission_measure(densitometer_t *densitometer, sensor_read_callback_t callback, void *user_data);

struct __densitometer_t {
    float last_d;
    float zero_d;
    const float max_d;
    const sensor_light_t read_light;
    const uint8_t read_light_idle_value;
    const densitometer_result_t (*measure_func)(densitometer_t *densitometer, sensor_read_callback_t callback, void *user_data);
};

static densitometer_t reflection_data = {
    .last_d = NAN,
    .zero_d = NAN,
    .max_d = REFLECTION_MAX_D,
    .read_light = SENSOR_LIGHT_REFLECTION,
    .read_light_idle_value = LIGHT_REFLECTION_IDLE,
    .measure_func = reflection_measure
};

static densitometer_t transmission_data = {
    .last_d = NAN,
    .zero_d = NAN,
    .max_d = TRANSMISSION_MAX_D,
    .read_light = SENSOR_LIGHT_TRANSMISSION,
    .read_light_idle_value = LIGHT_TRANSMISSION_IDLE,
    .measure_func = transmission_measure
};

static bool densitometer_allow_uncalibrated = false;

void densitometer_set_allow_uncalibrated_measurements(bool allow)
{
    densitometer_allow_uncalibrated = allow;
}

densitometer_t *densitometer_reflection()
{
    return &reflection_data;
}

densitometer_t *densitometer_transmission()
{
    return &transmission_data;
}

densitometer_result_t densitometer_measure(densitometer_t *densitometer, sensor_read_callback_t callback, void *user_data)
{
    if (!densitometer) { return DENSITOMETER_CAL_ERROR; }

    return densitometer->measure_func(densitometer, callback, user_data);
}

void densitometer_set_idle_light(const densitometer_t *densitometer, bool enabled)
{
    if (!densitometer) { return; }

    if (enabled) {
        sensor_set_light_mode(densitometer->read_light, false, densitometer->read_light_idle_value);
    } else {
        sensor_set_light_mode(SENSOR_LIGHT_OFF, false, 0);
    }
}

densitometer_result_t reflection_measure(densitometer_t *densitometer, sensor_read_callback_t callback, void *user_data)
{
    settings_cal_reflection_t cal_reflection;
    bool use_target_cal = true;

    /* Get the current calibration values */
    if (!settings_get_cal_reflection(&cal_reflection)) {
        if (densitometer_allow_uncalibrated) {
            use_target_cal = false;
        } else {
            return DENSITOMETER_CAL_ERROR;
        }
    }

    /* Perform sensor read */
    float ch0_basic;
    if (sensor_read_target(SENSOR_LIGHT_REFLECTION, &ch0_basic, NULL, callback, user_data) != osOK) {
        log_w("Sensor read error");
        sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, LIGHT_REFLECTION_IDLE);
        return DENSITOMETER_SENSOR_ERROR;
    }

    /* Combine and correct the basic reading */
    float corr_value = sensor_apply_slope_calibration(ch0_basic);

    if (use_target_cal) {
        /* Convert all values into log units */
        float meas_ll = log10f(corr_value);
        float cal_hi_ll = log10f(cal_reflection.hi_value);
        float cal_lo_ll = log10f(cal_reflection.lo_value);

        /* Calculate the slope of the line */
        float m = (cal_reflection.hi_d - cal_reflection.lo_d) / (cal_hi_ll - cal_lo_ll);

        /* Calculate the measured density */
        float meas_d = (m * (meas_ll - cal_lo_ll)) + cal_reflection.lo_d;

        log_i("D=%.2f, VALUE=%f,%f", meas_d, ch0_basic, corr_value);

        /* Clamp the return value to be within an acceptable range */
        if (meas_d <= 0.0F) { meas_d = 0.0F; }
        else if (meas_d > densitometer->max_d) { meas_d = densitometer->max_d; }

        densitometer->last_d = meas_d;

    } else {
        log_i("D=<uncal>, VALUE=%f,%f", ch0_basic, corr_value);

        /* Assign a default reading when missing target calibration */
        densitometer->last_d = 0.0F;
    }

    /* Set light back to idle */
    sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, LIGHT_REFLECTION_IDLE);

    if (cdc_is_connected()) {
        cdc_send_density_reading('R', densitometer->last_d, densitometer->zero_d, ch0_basic, corr_value);
    } else {
        hid_send_density_reading('R', densitometer->last_d, densitometer->zero_d);
    }

    return DENSITOMETER_OK;
}

densitometer_result_t transmission_measure(densitometer_t *densitometer, sensor_read_callback_t callback, void *user_data)
{
    settings_cal_transmission_t cal_transmission;
    bool use_target_cal = true;

    /* Get the current calibration values */
    if (!settings_get_cal_transmission(&cal_transmission)) {
        if (densitometer_allow_uncalibrated) {
            use_target_cal = false;
        } else {
            return DENSITOMETER_CAL_ERROR;
        }
    }

    /* Perform sensor read */
    float ch0_basic;
    if (sensor_read_target(SENSOR_LIGHT_TRANSMISSION, &ch0_basic, NULL, callback, user_data) != osOK) {
        log_w("Sensor read error");
        sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, LIGHT_TRANSMISSION_IDLE);
        return DENSITOMETER_SENSOR_ERROR;
    }

    /* Combine and correct the basic reading */
    float corr_value = sensor_apply_slope_calibration(ch0_basic);

    if (use_target_cal) {
        /* Calculate the measured CAL-HI density relative to the zero value */
        float cal_hi_meas_d = -1.0F * log10f(cal_transmission.hi_value / cal_transmission.zero_value);

        /* Calculate the measured target density relative to the zero value */
        float meas_d = -1.0F * log10f(corr_value / cal_transmission.zero_value);

        /* Calculate the adjustment factor */
        float adj_factor = cal_transmission.hi_d / cal_hi_meas_d;

        /* Calculate the calibration corrected density */
        float corr_d = meas_d * adj_factor;

        log_i("D=%.2f, VALUE=%f,%f", corr_d, ch0_basic, corr_value);

        /* Clamp the return value to be within an acceptable range */
        if (corr_d <= 0.0F) { corr_d = 0.0F; }
        else if (corr_d > densitometer->max_d) { corr_d = densitometer->max_d; }

        densitometer->last_d = corr_d;

    } else {
        log_i("D=<uncal>, VALUE=%f,%f", ch0_basic, corr_value);

        /* Assign a default reading when missing target calibration */
        densitometer->last_d = 0.0F;
    }

    /* Set light back to idle */
    sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, LIGHT_TRANSMISSION_IDLE);

    if (cdc_is_connected()) {
        cdc_send_density_reading('T', densitometer->last_d, densitometer->zero_d, ch0_basic, corr_value);
    } else {
        hid_send_density_reading('T', densitometer->last_d, densitometer->zero_d);
    }

    return DENSITOMETER_OK;
}

densitometer_result_t densitometer_calibrate(densitometer_t *densitometer, float *cal_value, sensor_read_callback_t callback, void *user_data)
{
    if (!densitometer) { return DENSITOMETER_CAL_ERROR; }

    /* Perform sensor read */
    float ch0_basic;
    if (sensor_read_target(densitometer->read_light, &ch0_basic, NULL, callback, user_data) != osOK) {
        log_w("Sensor read error");
        sensor_set_light_mode(densitometer->read_light, false, densitometer->read_light_idle_value);
        return DENSITOMETER_SENSOR_ERROR;
    }

    /* Combine and correct the basic reading */
    float corr_value = sensor_apply_slope_calibration(ch0_basic);

    if (ch0_basic < 0.0001F || corr_value < 0.0001F) {
        return DENSITOMETER_CAL_ERROR;
    }

    /* Assign the calibration value */
    if (cal_value) {
        *cal_value = corr_value;
    }

    /* Set light back to idle */
    sensor_set_light_mode(densitometer->read_light, false, densitometer->read_light_idle_value);

    return DENSITOMETER_OK;
}

void densitometer_set_zero_d(densitometer_t *densitometer, float d_value)
{
    if (!densitometer) { return; }

    if (!isnanf(d_value) && d_value >= 0.0F && d_value <= densitometer->max_d) {
        densitometer->zero_d = d_value;
    } else {
        densitometer->zero_d = NAN;
    }
}

float densitometer_get_zero_d(const densitometer_t *densitometer)
{
    if (!densitometer) { return NAN; }

    return densitometer->zero_d;
}

float densitometer_get_reading_d(const densitometer_t *densitometer)
{
    if (!densitometer) { return NAN; }

    return densitometer->last_d;
}

float densitometer_get_display_d(const densitometer_t *densitometer)
{
    if (!densitometer) { return NAN; }

    float display_value;
    if (!isnanf(densitometer->zero_d)) {
        display_value = densitometer->last_d - densitometer->zero_d;
    } else {
        display_value = densitometer->last_d;
    }

    /*
     * Clamp the return value to be within an acceptable range, allowing
     * for negative values as an indication to the user that their selected
     * offset might be inappropriate.
     */
    if (display_value > densitometer->max_d) {
        display_value = densitometer->max_d;
    } else if (display_value < (-1.0F * densitometer->max_d)) {
        display_value = -1.0F * densitometer->max_d;
    }

    return display_value;
}
