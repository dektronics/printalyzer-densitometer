#include "sensor.h"

#define LOG_TAG "sensor"

#include <elog.h>
#include <math.h>

#include "stm32l0xx_hal.h"
#include "settings.h"
#include "tsl2591.h"
#include "light.h"
#include "util.h"

#define SENSOR_DEFAULT_READ_TIME (TSL2591_TIME_200MS)

static I2C_HandleTypeDef *sensor_i2c = 0;

static HAL_StatusTypeDef sensor_wait_for_status(uint8_t flags, uint32_t timeout);
static HAL_StatusTypeDef sensor_clean_startup(tsl2591_time_t target_time, bool *is_saturated,
    sensor_read_callback_t callback, void *user_data);
static HAL_StatusTypeDef sensor_gain_calibration_loop(
    tsl2591_gain_t gain0, tsl2591_gain_t gain1, tsl2591_time_t time,
    float *gain_ch0, float *gain_ch1);
static HAL_StatusTypeDef sensor_read_loop(uint8_t count, uint8_t discard, uint16_t limit, float *ch0_avg, float *ch1_avg,
    sensor_read_callback_t callback, void *user_data);
static void sensor_convert_to_basic_counts(tsl2591_gain_t gain, tsl2591_time_t time,
    float ch0_val, float ch1_val, float *ch0_basic, float *ch1_basic);

HAL_StatusTypeDef sensor_init(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef ret = tsl2591_init(hi2c);
    if (ret == HAL_OK) {
        sensor_i2c  = hi2c;
    } else {
        log_e("Sensor initialization failed");
        sensor_i2c = 0;
    }
    return ret;
}

bool sensor_is_initialized()
{
    return sensor_i2c != 0;
}

HAL_StatusTypeDef sensor_wait_for_status(uint8_t flags, uint32_t timeout)
{
    uint32_t tick_start = HAL_GetTick();
    uint32_t ticks = 0;
    uint8_t status = 0;
    HAL_StatusTypeDef ret = HAL_OK;

    while (ret == HAL_OK && (status & flags) == 0) {
        ticks = HAL_GetTick();
        if ((ticks - tick_start) > timeout) {
            ret = HAL_TIMEOUT;
        } else {
            ret = tsl2591_get_status(sensor_i2c, &status);
        }
    }

    log_v("Waited %dms (%dms)", ticks - tick_start, timeout);

    return ret;
}

HAL_StatusTypeDef sensor_gain_calibration(sensor_gain_calibration_callback_t callback, void *user_data)
{
    /*
     * The sensor gain calibration process currently uses hand-picked
     * values for the integration time and transmission LED brightness.
     * Eventually, some mechanism for auto-ranging on the ideal LED
     * brightness for each step should be implemented, but the current
     * approach is likely good enough for now.
     */

    HAL_StatusTypeDef ret = HAL_OK;
    float gain_med_ch0 = NAN;
    float gain_med_ch1 = NAN;
    float gain_high_ch0 = NAN;
    float gain_high_ch1 = NAN;
    float gain_max_ch0 = NAN;
    float gain_max_ch1 = NAN;

    log_i("Starting gain calibration");

    if (callback) {
        callback(SENSOR_GAIN_CALIBRATION_STATUS_INIT, user_data);
    }

    /* Set lights to initial state */
    light_set_reflection(0);
    light_set_transmission(8);

    do {
        /* Put the sensor into a known initial state */
        ret = sensor_clean_startup(TSL2591_TIME_100MS, 0, NULL, NULL);
        if (ret != HAL_OK) { break; }

        /* Calibrate the value for medium gain */
        log_i("Medium gain calibration");
        if (callback) {
            callback(SENSOR_GAIN_CALIBRATION_STATUS_MEDIUM, user_data);
        }
        light_set_transmission(128);
        ret = sensor_gain_calibration_loop(TSL2591_GAIN_LOW, TSL2591_GAIN_MEDIUM, TSL2591_TIME_600MS, &gain_med_ch0, &gain_med_ch1);
        if (ret != HAL_OK) { break; }

        log_i("Medium gain: CH0=%dx, CH1=%dx", lroundf(gain_med_ch0), lroundf(gain_med_ch1));

        if (gain_med_ch0 < TSL2591_GAIN_MEDIUM_MIN || gain_med_ch0 > TSL2591_GAIN_MEDIUM_MAX) {
            log_w("Medium CH0 gain out of range!");
            gain_med_ch0 = TSL2591_GAIN_MEDIUM_TYP;
        }
        if (gain_med_ch1 < TSL2591_GAIN_MEDIUM_MIN || gain_med_ch1 > TSL2591_GAIN_MEDIUM_MAX) {
            log_w("Medium CH1 gain out of range!");
            gain_med_ch1 = TSL2591_GAIN_MEDIUM_TYP;
        }

        /* Calibrate the value for high gain */
        log_i("High gain calibration");
        if (callback) {
            callback(SENSOR_GAIN_CALIBRATION_STATUS_HIGH, user_data);
        }
        light_set_transmission(35);
        ret = sensor_gain_calibration_loop(TSL2591_GAIN_MEDIUM, TSL2591_GAIN_HIGH, TSL2591_TIME_200MS, &gain_high_ch0, &gain_high_ch1);
        if (ret != HAL_OK) { break; }

        gain_high_ch0 *= gain_med_ch0;
        gain_high_ch1 *= gain_med_ch1;

        log_i("High gain: CH0=%dx, CH1=%dx", lroundf(gain_high_ch0), lroundf(gain_high_ch1));

        if (gain_high_ch0 < TSL2591_GAIN_HIGH_MIN || gain_high_ch0 > TSL2591_GAIN_HIGH_MAX) {
            log_w("High CH0 gain out of range!");
            gain_high_ch0 = TSL2591_GAIN_HIGH_TYP;
        }
        if (gain_high_ch1 < TSL2591_GAIN_HIGH_MIN || gain_high_ch1 > TSL2591_GAIN_HIGH_MAX) {
            log_w("High CH1 gain out of range!");
            gain_high_ch1 = TSL2591_GAIN_HIGH_TYP;
        }

        /* Calibrate the value for maximum gain */
        log_i("Maximum gain calibration");
        if (callback) {
            callback(SENSOR_GAIN_CALIBRATION_STATUS_MAXIMUM, user_data);
        }
        light_set_transmission(5);
        ret = sensor_gain_calibration_loop(TSL2591_GAIN_HIGH, TSL2591_GAIN_MAXIMUM, TSL2591_TIME_200MS, &gain_max_ch0, &gain_max_ch1);
        if (ret != HAL_OK) { break; }

        gain_max_ch0 *= gain_high_ch0;
        gain_max_ch1 *= gain_high_ch1;

        log_i("Maximum gain: CH0=%dx, CH1=%dx", lroundf(gain_max_ch0), lroundf(gain_max_ch1));

        if (gain_max_ch0 < TSL2591_GAIN_MAXIMUM_CH0_MIN || gain_max_ch0 > TSL2591_GAIN_MAXIMUM_CH0_MAX) {
            log_w("Maximum CH0 gain out of range!");
            gain_max_ch0 = TSL2591_GAIN_MAXIMUM_CH0_TYP;
        }
        if (gain_max_ch1 < TSL2591_GAIN_MAXIMUM_CH1_MIN || gain_max_ch1 > TSL2591_GAIN_MAXIMUM_CH1_MAX) {
            log_w("Maximum CH1 gain out of range!");
            gain_max_ch1 = TSL2591_GAIN_MAXIMUM_CH1_TYP;
        }
    } while (0);

    if (callback) {
        if (ret == HAL_OK) {
            callback(SENSOR_GAIN_CALIBRATION_STATUS_DONE, user_data);
        } else {
            callback(SENSOR_GAIN_CALIBRATION_STATUS_FAILED, user_data);
        }
    }

    /* Turn off the sensor */
    tsl2591_set_enable(sensor_i2c, 0x00);

    /* Turn off the lights */
    light_set_transmission(0);

    if (ret == HAL_OK) {
        log_i("Gain calibration complete");
        settings_set_cal_gain(TSL2591_GAIN_MEDIUM, gain_med_ch0, gain_med_ch1);
        settings_set_cal_gain(TSL2591_GAIN_HIGH, gain_high_ch0, gain_high_ch1);
        settings_set_cal_gain(TSL2591_GAIN_MAXIMUM, gain_max_ch1, gain_max_ch1);
    } else {
        log_e("Gain calibration failed");
    }

    return ret;
}

HAL_StatusTypeDef sensor_read(uint8_t iterations, float *ch0_result, float *ch1_result, sensor_read_callback_t callback, void *user_data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    tsl2591_gain_t gain = TSL2591_GAIN_MAXIMUM;
    tsl2591_time_t time = SENSOR_DEFAULT_READ_TIME;
    uint8_t discard = 0;
    float ch0_avg = NAN;
    float ch1_avg = NAN;
    bool is_saturated = false;

    log_i("Starting reflection read, %d iterations", iterations);

    do {
        /* Put the sensor into a known initial state, with maximum gain */
        ret = sensor_clean_startup(time, &is_saturated, callback, user_data);
        if (ret != HAL_OK) { break; }

        do {
            if (is_saturated) {
                if (gain == TSL2591_GAIN_LOW) { break; }
                gain--;

                /* Set the new gain value */
                ret = tsl2591_set_config(sensor_i2c, gain, time);
                if (ret != HAL_OK) { break; }

                /* Discard the first result after reading again */
                discard = 1;
                is_saturated = false;
            }

            /* Run the read loop */
            ret = sensor_read_loop(iterations, discard, TSL2591_DIGITAL_SATURATION, &ch0_avg, &ch1_avg, callback, user_data);
            if (ret != HAL_OK) { break; }

            /* Check for saturation */
            if (isnanf(ch0_avg) || isnanf(ch1_avg)) {
                is_saturated = true;
            }

        } while (is_saturated && ret == HAL_OK);

    } while (0);

    /* Turn off the sensor */
    tsl2591_set_enable(sensor_i2c, 0x00);

    if (!is_saturated && ret == HAL_OK) {
        log_i("Sensor read complete");
        uint16_t time_ms = tsl2591_get_time_value_ms(time);
        log_d("TSL2591: CH0=%d, CH1=%d, Gain=[%d], Time=%dms", lroundf(ch0_avg), lroundf(ch1_avg), gain, time_ms);
        sensor_convert_to_basic_counts(gain, time, ch0_avg, ch1_avg, ch0_result, ch1_result);
    } else {
        log_e("Sensor read failed: is_saturated=%d, ret=%d", is_saturated, ret);
        if (ret == HAL_OK) {
            ret = HAL_ERROR;
        }
    }

    return ret;
}

/**
 * Start up the sensor and get it into a known good state.
 *
 * This process is based on the routine outlined in the
 * application note titled:
 * "Accurate ADC Readings after Enable (DN42)"
 *
 * That note is written assuming the quirks of a slightly different sensor,
 * and doesn't clearly cover how to pivot to different gain and integration
 * time settings once operational. Its also possible that this entire routine
 * is mostly unnecessary if the first few readings are discarded anyways.
 * Regardless, it is a good initialization workout.
 *
 * @param target_time Integration time to bring the sensor to at the end of startup
 * @param is_saturated Set to true if the sensor saturates at maximum gain
 */
HAL_StatusTypeDef sensor_clean_startup(tsl2591_time_t target_time, bool *is_saturated, sensor_read_callback_t callback, void *user_data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint16_t ch0_val = 0;
    uint16_t ch1_val = 0;

    if (callback) { callback(user_data); }

    do {
        /* Put the sensor into a known initial state */
        ret = tsl2591_set_enable(sensor_i2c, 0x00);
        if (ret != HAL_OK) { break; }

        tsl2591_clear_als_int(sensor_i2c);
        if (ret != HAL_OK) { break; }

        /* Power on the sensor */
        ret = tsl2591_set_enable(sensor_i2c, TSL2591_ENABLE_PON);
        if (ret != HAL_OK) { break; }

        /* Set the maximum gain and minimum integration time */
        ret = tsl2591_set_config(sensor_i2c, TSL2591_GAIN_MAXIMUM, TSL2591_TIME_100MS);
        if (ret != HAL_OK) { break; }

        /* Interrupt after every integration cycle */
        ret = tsl2591_set_persist(sensor_i2c, TSL2591_PERSIST_EVERY);
        if (ret != HAL_OK) { break; }

        /* Enable ALS with interrupts */
        ret = tsl2591_set_enable(sensor_i2c, TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN | TSL2591_ENABLE_AIEN);
        if (ret != HAL_OK) { break; }

        /* Change the target integration time if necessary */
        if (target_time != TSL2591_TIME_100MS) {
            ret = tsl2591_set_config(sensor_i2c, TSL2591_GAIN_MAXIMUM, target_time);
            if (ret != HAL_OK) { break; }
        }

        /* Wait for the first interrupt */
        ret = sensor_wait_for_status(TSL2591_STATUS_AINT, 2000);
        if (ret != HAL_OK) { break; }

        /* Clear the interrupt */
        ret = tsl2591_clear_als_int(sensor_i2c);
        if (ret != HAL_OK) { break; }

        if (callback) { callback(user_data); }

        /* Wait for the second interrupt */
        ret = sensor_wait_for_status(TSL2591_STATUS_AINT, 2000);
        if (ret != HAL_OK) { break; }

        /* Clear and disable the interrupt */
        ret = tsl2591_set_enable(sensor_i2c, TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN);
        if (ret != HAL_OK) { break; }

        ret = tsl2591_clear_als_int(sensor_i2c);
        if (ret != HAL_OK) { break; }

        if (callback) { callback(user_data); }

        /* Read full channel data */
        ret = tsl2591_get_full_channel_data(sensor_i2c, &ch0_val, &ch1_val);
        if (ret != HAL_OK) { break; }

    } while (0);

    if (ret == HAL_OK) {
        if (is_saturated) {
            uint16_t limit;
            if (target_time == TSL2591_TIME_100MS) {
                limit = TSL2591_ANALOG_SATURATION;
            } else {
                limit = TSL2591_DIGITAL_SATURATION;
            }
            if (ch0_val >= limit || ch1_val >= limit) {
                *is_saturated = true;
            } else {
                *is_saturated = false;
            }
        }
    } else {
        tsl2591_set_enable(sensor_i2c, 0x00);
        log_e("Sensor clean startup failed: %d", ret);
    }

    return ret;
}

/**
 * Sensor read loop.
 *
 * Assumes the sensor is already running and configured.
 *
 * @param count Number of values to average
 * @param discard Number of initial values to discard
 * @param limit Saturation limit, which will cause the loop to exit and results to be NAN
 * @param ch0_avg Average reading of Channel 0
 * @param ch1_avg Average reading of Channel 1
 */
HAL_StatusTypeDef sensor_read_loop(uint8_t count, uint8_t discard, uint16_t limit, float *ch0_avg, float *ch1_avg,
    sensor_read_callback_t callback, void *user_data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint16_t ch0_val = 0;
    uint16_t ch1_val = 0;
    float ch0_sum = 0;
    float ch1_sum = 0;
    bool saturation = false;

    if (count == 0) {
        return HAL_ERROR;
    }

    do {
        /* Clear and disable the interrupt */
        ret = tsl2591_set_enable(sensor_i2c, TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN);
        if (ret != HAL_OK) { break; }

        ret = tsl2591_clear_als_int(sensor_i2c);
        if (ret != HAL_OK) { break; }

        /* Configure the interrupt to be raised after every integration cycle */
        ret = tsl2591_set_persist(sensor_i2c, TSL2591_PERSIST_EVERY);
        if (ret != HAL_OK) { break; }

        /* Enable ALS with interrupts */
        ret = tsl2591_set_enable(sensor_i2c, TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN | TSL2591_ENABLE_AIEN);
        if (ret != HAL_OK) { break; }

        /* Loop over measurements, discarding the initial ones */
        for (uint8_t i = 0; i < count + discard; i++) {
            /* Wait for the interrupt flag */
            ret = sensor_wait_for_status(TSL2591_STATUS_AINT, 2000);
            if (ret != HAL_OK) { break; }

            /* Clear the interrupt */
            ret = tsl2591_clear_als_int(sensor_i2c);
            if (ret != HAL_OK) { break; }

            if (callback) { callback(user_data); }

            /* Read full channel data */
            ret = tsl2591_get_full_channel_data(sensor_i2c, &ch0_val, &ch1_val);
            if (ret != HAL_OK) { break; }

            /* Accumulate the results */
            if (i >= discard) {
                log_v("TSL2591[%d]: CH0=%d, CH1=%d", i - discard, lroundf(ch0_val), lroundf(ch1_val));
                if (limit > 0 && (ch0_val >= limit || ch1_val >= limit)) {
                    log_w("Sensor value over limit: CH0: %d >= %d, CH1: %d >= %d",
                        ch0_val, limit, ch1_val, limit);
                    saturation = true;
                    break;
                }
                ch0_sum += logf(ch0_val);
                ch1_sum += logf(ch1_val);
            }
        }
        if (ret != HAL_OK) { break; }

        /* Clear and disable the interrupt */
        ret = tsl2591_set_enable(sensor_i2c, TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN);
        if (ret != HAL_OK) { break; }

        ret = tsl2591_clear_als_int(sensor_i2c);
        if (ret != HAL_OK) { break; }

    } while (0);

    if (ret == HAL_OK) {
        if (ch0_avg) {
            *ch0_avg = saturation ? NAN : expf((float)ch0_sum / (float)count);
        }
        if (ch1_avg) {
            *ch1_avg = saturation ? NAN : expf((float)ch1_sum / (float)count);
        }
    } else {
        tsl2591_set_enable(sensor_i2c, 0x00);
        log_e("Sensor error during read loop: %d", ret);
    }

    return ret;
}

/**
 * Calibrate the relationship between two gain values.
 *
 * @param gain_low Low gain value
 * @param gain_high High gain value
 * @param time Integration time for measurements
 * @param gain_ch0 Measured gain for Channel 0
 * @param gain_ch0 Measured gain for Channel 1
 */
HAL_StatusTypeDef sensor_gain_calibration_loop(
    tsl2591_gain_t gain_low, tsl2591_gain_t gain_high, tsl2591_time_t time,
    float *gain_ch0, float *gain_ch1)
{
    HAL_StatusTypeDef ret = HAL_OK;
    float ch0_avg_high;
    float ch1_avg_high;
    float ch0_avg_low;
    float ch1_avg_low;

    if (gain_low >= gain_high) {
        return HAL_ERROR;
    }

    do {
        /* Setup for high gain measurement */
        ret = tsl2591_set_config(sensor_i2c, gain_high, time);
        if (ret != HAL_OK) { break; }

        /* Do the high gain read loop */
        log_d("Higher gain loop...");
        ret = sensor_read_loop(5, 2, 0, &ch0_avg_high, &ch1_avg_high, NULL, NULL);
        if (ret != HAL_OK) { break; }

        log_d("TSL2591[Higher]: CH0=%d, CH1=%d", lroundf(ch0_avg_high), lroundf(ch1_avg_high));

        /* Setup for low gain measurement */
        ret = tsl2591_set_config(sensor_i2c, gain_low, time);
        if (ret != HAL_OK) { break; }

        /* Do the low gain read loop */
        log_d("Lower gain loop...");
        ret = sensor_read_loop(5, 2, 0, &ch0_avg_low, &ch1_avg_low, NULL, NULL);
        if (ret != HAL_OK) { break; }

        log_d("TSL2591[Lower]: CH0=%d, CH1=%d", lroundf(ch0_avg_low), lroundf(ch1_avg_low));
    } while (0);

    if (ret == HAL_OK) {
        if (gain_ch0) {
            if (ch0_avg_high <= 0 || ch0_avg_low <= 0) {
                *gain_ch0 = 0;
            } else {
                *gain_ch0 = ch0_avg_high / ch0_avg_low;
            }
        }
        if (gain_ch1) {
            if (ch1_avg_high <= 0 || ch1_avg_low <= 0) {
                *gain_ch1 = 0;
            } else {
                *gain_ch1 = ch1_avg_high / ch1_avg_low;
            }
        }
    }

    return ret;
}

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
void sensor_convert_to_basic_counts(tsl2591_gain_t gain, tsl2591_time_t time, float ch0_val, float ch1_val, float *ch0_basic, float *ch1_basic)
{
    float ch0_gain;
    float ch1_gain;
    uint16_t atime_ms = tsl2591_get_time_value_ms(time);
    settings_get_cal_gain(gain, &ch0_gain, &ch1_gain);

    float ch0_cpl = (atime_ms * ch0_gain) / (TSL2591_LUX_GA * TSL2591_LUX_DF);
    float ch1_cpl = (atime_ms * ch1_gain) / (TSL2591_LUX_GA * TSL2591_LUX_DF);

    if (ch0_basic) {
        *ch0_basic = ch0_val / ch0_cpl;
    }
    if (ch1_basic) {
        *ch1_basic = ch1_val / ch1_cpl;
    }
}
