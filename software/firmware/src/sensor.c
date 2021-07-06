#include "sensor.h"

#define LOG_TAG "sensor"

#include <elog.h>
#include <math.h>

#include "stm32l0xx_hal.h"
#include "settings.h"
#include "tsl2591.h"
#include "light.h"

I2C_HandleTypeDef *sensor_i2c = 0;

static HAL_StatusTypeDef sensor_wait_for_status(uint8_t flags, uint32_t timeout);
static HAL_StatusTypeDef sensor_clean_startup();
static HAL_StatusTypeDef sensor_gain_calibration_loop(
    tsl2591_gain_t gain0, tsl2591_gain_t gain1, tsl2591_time_t time,
    float *gain_ch0, float *gain_ch1);
HAL_StatusTypeDef sensor_read_loop(uint8_t count, uint8_t discard, float *ch0_avg, float *ch1_avg);

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

    //log_v("Waited %dms", ticks - tick_start); //XXX

    return ret;
}

/*
 * Saturation notes:
 * Gain = Max, Int = 100ms, Analog Saturation = 37888
 * I think analog saturation can only happen at a 100ms integration time.
 * Above that, its digital saturation at 65535
 */

HAL_StatusTypeDef sensor_gain_calibration()
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

    /* Set lights to initial state */
    light_set_reflection(0);
    light_set_transmission(8);

    do {
        /* Put the sensor into a known initial state */
        ret = sensor_clean_startup();
        if (ret != HAL_OK) { break; }

        /* Calibrate the value for medium gain */
        log_i("Medium gain calibration");
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

    /* Turn off the sensor */
    tsl2591_set_enable(sensor_i2c, 0x00);

    /* Turn off the lights */
    light_set_transmission(0);

    if (ret == HAL_OK) {
        log_i("Gain calibration complete");
        settings_set_calibration_gain(TSL2591_GAIN_MEDIUM, gain_med_ch0, gain_med_ch1);
        settings_set_calibration_gain(TSL2591_GAIN_HIGH, gain_high_ch0, gain_high_ch1);
        settings_set_calibration_gain(TSL2591_GAIN_MAXIMUM, gain_max_ch1, gain_max_ch1);
    } else {
        log_e("Gain calibration failed");
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
 */
HAL_StatusTypeDef sensor_clean_startup()
{
    HAL_StatusTypeDef ret = HAL_OK;

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

        /* Set thresholds to ensure an interrupt */
        ret = tsl2591_set_als_low_int_threshold(sensor_i2c, 0x0000);
        if (ret != HAL_OK) { break; }

        ret = tsl2591_set_als_high_int_threshold(sensor_i2c, 0x0000);
        if (ret != HAL_OK) { break; }

        /* Interrupt after two integration cycles out of range */
        ret = tsl2591_set_persist(sensor_i2c, TSL2591_PERSIST_2);
        if (ret != HAL_OK) { break; }

        /* Enable ALS with interrupts */
        ret = tsl2591_set_enable(sensor_i2c, TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN | TSL2591_ENABLE_AIEN);
        if (ret != HAL_OK) { break; }

        /* Wait for the interrupt flag */
        ret = sensor_wait_for_status(TSL2591_STATUS_AINT, 500);
        if (ret != HAL_OK) { break; }

        /* Clear and disable the interrupt */
        ret = tsl2591_set_enable(sensor_i2c, TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN);
        if (ret != HAL_OK) { break; }

        ret = tsl2591_clear_als_int(sensor_i2c);
        if (ret != HAL_OK) { break; }

        /* Read full channel data */
        ret = tsl2591_get_full_channel_data(sensor_i2c, 0, 0);
        if (ret != HAL_OK) { break; }

    } while (0);

    if (ret != HAL_OK) {
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
 * @param ch0_avg Average reading of Channel 0
 * @param ch1_avg Average reading of Channel 1
 */
HAL_StatusTypeDef sensor_read_loop(uint8_t count, uint8_t discard, float *ch0_avg, float *ch1_avg)
{
    //TODO Add a way to check for saturation
    HAL_StatusTypeDef ret = HAL_OK;
    uint16_t ch0_val = 0;
    uint16_t ch1_val = 0;
    uint32_t ch0_sum = 0;
    uint32_t ch1_sum = 0;

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
            ret = sensor_wait_for_status(TSL2591_STATUS_AINT, 750);
            if (ret != HAL_OK) { break; }

            /* Clear the interrupt */
            ret = tsl2591_clear_als_int(sensor_i2c);
            if (ret != HAL_OK) { break; }

            /* Read full channel data */
            ret = tsl2591_get_full_channel_data(sensor_i2c, &ch0_val, &ch1_val);
            if (ret != HAL_OK) { break; }

            /* Accumulate the results */
            if (i >= discard) {
                ch0_sum += ch0_val;
                ch1_sum += ch1_val;
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
            *ch0_avg = (float)ch0_sum / (float)count;
        }
        if (ch1_avg) {
            *ch1_avg = (float)ch1_sum / (float)count;
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
        ret = sensor_read_loop(5, 2, &ch0_avg_high, &ch1_avg_high);
        if (ret != HAL_OK) { break; }

        log_d("TSL2591[Higher]: CH0=%d, CH1=%d", lroundf(ch0_avg_high), lroundf(ch1_avg_high));

        /* Setup for low gain measurement */
        ret = tsl2591_set_config(sensor_i2c, gain_low, time);
        if (ret != HAL_OK) { break; }

        /* Do the low gain read loop */
        log_d("Lower gain loop...");
        ret = sensor_read_loop(5, 2, &ch0_avg_low, &ch1_avg_low);
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
