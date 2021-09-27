#include "sensor.h"

#define LOG_TAG "sensor"

#include <elog.h>
#include <math.h>
#include <cmsis_os.h>
#include <FreeRTOS.h>
#include <queue.h>

#include "stm32l0xx_hal.h"
#include "settings.h"
#include "tsl2591.h"
#include "light.h"
#include "util.h"

#define SENSOR_DEFAULT_READ_TIME (TSL2591_TIME_200MS)

extern I2C_HandleTypeDef hi2c1;

typedef enum {
    SENSOR_CONTROL_STOP = 0,
    SENSOR_CONTROL_START,
    SENSOR_CONTROL_SET_CONFIG,
    SENSOR_CONTROL_INTERRUPT
} sensor_control_event_type_t;

typedef struct {
    sensor_control_event_type_t event_type;
    uint8_t param1;
    uint8_t param2;
} sensor_control_event_t;

static volatile bool sensor_initialized = false;

static bool sensor_running = false;
static tsl2591_gain_t sensor_gain = TSL2591_GAIN_LOW;
static tsl2591_time_t sensor_time = TSL2591_TIME_100MS;
static bool sensor_discard_next_reading = false;

/* Queue for low level sensor control events */
static osMessageQueueId_t sensor_control_queue = NULL;
static const osMessageQueueAttr_t sensor_control_queue_attrs = {
    .name = "sensor_control_queue"
};

/* Queue to hold the latest sensor reading */
static osMessageQueueId_t sensor_reading_queue = NULL;
static const osMessageQueueAttr_t sensor_reading_queue_attrs = {
    .name = "sensor_reading_queue"
};

/* Semaphore to synchronize sensor control calls */
static osSemaphoreId_t sensor_control_semaphore = NULL;
static const osSemaphoreAttr_t sensor_control_semaphore_attrs = {
    .name = "sensor_control_semaphore"
};

static void sensor_control_start();
static void sensor_control_stop();
static void sensor_control_set_config(uint8_t param1, uint8_t param2);
static void sensor_control_interrupt();

static osStatus_t sensor_gain_calibration_loop(
    tsl2591_gain_t gain0, tsl2591_gain_t gain1, tsl2591_time_t time,
    float *gain_ch0, float *gain_ch1);
static osStatus_t sensor_read_loop(uint8_t count, float *ch0_avg, float *ch1_avg,
    sensor_read_callback_t callback, void *user_data);

void task_sensor_run(void *argument)
{
    osSemaphoreId_t task_start_semaphore = argument;
    HAL_StatusTypeDef ret;
    sensor_control_event_t control_event;

    log_d("sensor_task start");

    /* Create the queue for sensor control events */
    sensor_control_queue = osMessageQueueNew(20, sizeof(sensor_control_event_t), &sensor_control_queue_attrs);
    if (!sensor_control_queue) {
        log_e("Unable to create control queue");
        return;
    }

    /* Create the one-element queue to hold the latest sensor reading */
    sensor_reading_queue = osMessageQueueNew(1, sizeof(sensor_reading_t), &sensor_reading_queue_attrs);
    if (!sensor_reading_queue) {
        log_e("Unable to create reading queue");
        return;
    }

    /* Create the semaphore used to synchronize sensor control */
    sensor_control_semaphore = osSemaphoreNew(1, 0, &sensor_control_semaphore_attrs);
    if (!sensor_control_semaphore) {
        log_e("sensor_control_semaphore create error");
        return;
    }

    ret = tsl2591_init(&hi2c1);
    if (ret != HAL_OK) {
        log_e("Sensor initialization failed");
        return;
    }

    /* Set the initialized flag */
    sensor_initialized = true;

    /* Release the startup semaphore */
    if (osSemaphoreRelease(task_start_semaphore) != osOK) {
        log_e("Unable to release task_start_semaphore");
        return;
    }

    /* Start the main control event loop */
    for (;;) {
        if(osMessageQueueGet(sensor_control_queue, &control_event, NULL, portMAX_DELAY) == osOK) {
            switch (control_event.event_type) {
            case SENSOR_CONTROL_STOP:
                sensor_control_stop();
                break;
            case SENSOR_CONTROL_START:
                sensor_control_start();
                break;
            case SENSOR_CONTROL_SET_CONFIG:
                sensor_control_set_config(control_event.param1, control_event.param2);
                break;
            case SENSOR_CONTROL_INTERRUPT:
                sensor_control_interrupt();
                break;
            default:
                break;
            }
        }
    }
}

void sensor_start()
{
    sensor_control_event_t control_event = {
        .event_type = SENSOR_CONTROL_START
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, portMAX_DELAY);

    if (osSemaphoreAcquire(sensor_control_semaphore, portMAX_DELAY) != osOK) {
        log_e("Unable to acquire sensor_control_semaphore");
    }
}

void sensor_stop()
{
    sensor_control_event_t control_event = {
        .event_type = SENSOR_CONTROL_STOP
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, portMAX_DELAY);

    if (osSemaphoreAcquire(sensor_control_semaphore, portMAX_DELAY) != osOK) {
        log_e("Unable to acquire sensor_control_semaphore");
    }
}

void sensor_set_config(tsl2591_gain_t gain, tsl2591_time_t time)
{
    sensor_control_event_t control_event = {
        .event_type = SENSOR_CONTROL_SET_CONFIG,
        .param1 = gain,
        .param2 = time
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, portMAX_DELAY);

    if (osSemaphoreAcquire(sensor_control_semaphore, portMAX_DELAY) != osOK) {
        log_e("Unable to acquire sensor_control_semaphore");
    }
}

osStatus_t sensor_get_next_reading(sensor_reading_t *reading, uint32_t timeout)
{
    if (!reading) {
        return osErrorParameter;
    }

    return osMessageQueueGet(sensor_reading_queue, reading, NULL, timeout);
}

void sensor_control_start()
{
    HAL_StatusTypeDef ret = HAL_OK;

    log_d("sensor_control_start");

    do {
        /* Put the sensor into a known initial state */
        ret = tsl2591_set_enable(&hi2c1, 0x00);
        if (ret != HAL_OK) { break; }

        sensor_running = false;

        /* Clear any pending interrupt flags */
        tsl2591_clear_als_int(&hi2c1);
        if (ret != HAL_OK) { break; }

        /* Power on the sensor */
        ret = tsl2591_set_enable(&hi2c1, TSL2591_ENABLE_PON);
        if (ret != HAL_OK) { break; }

        /* Set the maximum gain and minimum integration time */
        ret = tsl2591_set_config(&hi2c1, sensor_gain, sensor_time);
        if (ret != HAL_OK) { break; }

        /* Interrupt after every integration cycle */
        ret = tsl2591_set_persist(&hi2c1, TSL2591_PERSIST_EVERY);
        if (ret != HAL_OK) { break; }

        /* Clear out any old sensor readings */
        osMessageQueueReset(sensor_reading_queue);

        /* Enable ALS with interrupts */
        ret = tsl2591_set_enable(&hi2c1, TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN | TSL2591_ENABLE_AIEN);
        if (ret != HAL_OK) { break; }

        sensor_discard_next_reading = true;
        sensor_running = true;
    } while (0);

    if (osSemaphoreRelease(sensor_control_semaphore) != osOK) {
        log_e("Unable to release sensor_control_semaphore");
    }
}

void sensor_control_stop()
{
    log_d("sensor_control_stop");
    if (tsl2591_set_enable(&hi2c1, 0x00) == HAL_OK) {
        sensor_running = false;
    }

    if (osSemaphoreRelease(sensor_control_semaphore) != osOK) {
        log_e("Unable to release sensor_control_semaphore");
    }
}

void sensor_control_set_config(uint8_t param1, uint8_t param2)
{
    tsl2591_gain_t pending_gain = param1;
    tsl2591_time_t pending_time = param2;

    log_d("sensor_control_set_config: %d, %d", pending_gain, pending_time);

    if (sensor_running) {
        if (tsl2591_set_config(&hi2c1, param1, param2) == HAL_OK) {
            sensor_gain = pending_gain;
            sensor_time = pending_time;
            sensor_discard_next_reading = true;
            osMessageQueueReset(sensor_reading_queue);
        }
    } else {
        sensor_gain = pending_gain;
        sensor_time = pending_time;
    }

    if (osSemaphoreRelease(sensor_control_semaphore) != osOK) {
        log_e("Unable to release sensor_control_semaphore");
    }
}

void sensor_control_interrupt()
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t status = 0;
    sensor_reading_t reading = {0};
    bool has_channel_data = false;

    //log_d("sensor_control_interrupt");

    if (!sensor_running) {
        log_w("Unexpected sensor interrupt!");
    }

    do {
        /* Get the status register */
        ret = tsl2591_get_status(&hi2c1, &status);
        if (ret != HAL_OK) { break; }

        /* Make sure we actually triggered the ALS interrupt */
        if ((status & TSL2591_STATUS_AINT) == 0) {
            break;
        }

        /* Clear the ALS interrupt */
        ret = tsl2591_clear_als_int(&hi2c1);
        if (ret != HAL_OK) { break; }

        if (sensor_discard_next_reading) {
            sensor_discard_next_reading = false;
            break;
        }

        /* Read full channel data */
        ret = tsl2591_get_full_channel_data(&hi2c1, &reading.ch0_val, &reading.ch1_val);
        if (ret != HAL_OK) { break; }

        /* Fill out other reading fields */
        reading.gain = sensor_gain;
        reading.time = sensor_time;

        has_channel_data = true;
    } while (0);

    //TODO need to make sure we can't jam in some of the failure cases

    if (has_channel_data) {
        log_d("TSL2591: CH0=%d, CH1=%d, Gain=[%d], Time=%dms", reading.ch0_val, reading.ch1_val, sensor_gain, tsl2591_get_time_value_ms(sensor_time));

        QueueHandle_t queue = (QueueHandle_t)sensor_reading_queue;
        xQueueOverwrite(queue, &reading);
    }
}

bool sensor_is_initialized()
{
    return sensor_initialized;
}

void sensor_int_handler()
{
    sensor_control_event_t control_event = {
        .event_type = SENSOR_CONTROL_INTERRUPT
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, 0);
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

    osStatus_t ret = osOK;
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
        sensor_set_config(TSL2591_GAIN_MAXIMUM, TSL2591_TIME_100MS);
        sensor_start();

        /* Wait for things to stabilize */
        osDelay(1000);

        /* Calibrate the value for medium gain */
        log_i("Medium gain calibration");
        if (callback) {
            callback(SENSOR_GAIN_CALIBRATION_STATUS_MEDIUM, user_data);
        }
        light_set_transmission(128);
        ret = sensor_gain_calibration_loop(TSL2591_GAIN_LOW, TSL2591_GAIN_MEDIUM, TSL2591_TIME_600MS, &gain_med_ch0, &gain_med_ch1);
        if (ret != osOK) { break; }

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
        if (ret != osOK) { break; }

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
        if (ret != osOK) { break; }

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
        if (ret == osOK) {
            callback(SENSOR_GAIN_CALIBRATION_STATUS_DONE, user_data);
        } else {
            callback(SENSOR_GAIN_CALIBRATION_STATUS_FAILED, user_data);
        }
    }

    /* Turn off the sensor */
    sensor_stop();

    /* Turn off the lights */
    light_set_transmission(0);

    if (ret == osOK) {
        log_i("Gain calibration complete");

        char numbuf0[16];
        char numbuf1[16];

        log_d("Low -> 1.000000 1.000000");

        float_to_str(gain_med_ch0, numbuf0, 6);
        float_to_str(gain_med_ch1, numbuf1, 6);
        log_d("Med -> %s %s", numbuf0, numbuf1);

        float_to_str(gain_high_ch0, numbuf0, 6);
        float_to_str(gain_high_ch1, numbuf1, 6);
        log_d("High -> %s %s", numbuf0, numbuf1);

        float_to_str(gain_max_ch0, numbuf0, 6);
        float_to_str(gain_max_ch1, numbuf1, 6);
        log_d("Max -> %s %s", numbuf0, numbuf1);

        settings_set_cal_gain(TSL2591_GAIN_MEDIUM, gain_med_ch0, gain_med_ch1);
        settings_set_cal_gain(TSL2591_GAIN_HIGH, gain_high_ch0, gain_high_ch1);
        settings_set_cal_gain(TSL2591_GAIN_MAXIMUM, gain_max_ch0, gain_max_ch1);
    } else {
        log_e("Gain calibration failed");
    }

    return (ret == osOK) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef sensor_time_calibration(sensor_time_calibration_callback_t callback, void *user_data)
{
    osStatus_t ret = osOK;
    float ch0_readings[6];

    log_i("Starting integration time calibration");

    /* Set lights to initial state */
    light_set_reflection(0);
    light_set_transmission(128);

    do {
        /* Put the sensor into a known initial state */
        sensor_set_config(TSL2591_GAIN_MAXIMUM, TSL2591_TIME_100MS);
        sensor_start();

        /* Wait for things to stabilize */
        osDelay(1000);

        /* Loop over each integration time and collect the average measurement on CH0 */
        for (tsl2591_time_t time = TSL2591_TIME_100MS; time <= TSL2591_TIME_600MS; time++) {
            float ch0_avg;

            log_i("Measuring %dms integration", tsl2591_get_time_value_ms(time));
            if (callback) { callback(time, user_data); }

            /* Set integration time for measurement */
            sensor_set_config(TSL2591_GAIN_MEDIUM, time);

            /* Take an average measurement at the specified time */
            ret = sensor_read_loop(5, &ch0_avg, NULL, NULL, NULL);
            if (ret != osOK) { break; }

            /* Check for a bad result */
            if (ch0_avg <= 0.0F || isnanf(ch0_avg)) {
                ret = osError;
                break;
            }
            ch0_readings[time] = ch0_avg;
        }
    } while (0);

    /* Turn off the sensor */
    sensor_stop();

    /* Turn off the lights */
    light_set_transmission(0);

    if (ret == osOK) {
        log_i("Integration time calibration complete");
        float measured_time[6];
        char numbuf[16];
        for (tsl2591_time_t time = TSL2591_TIME_100MS; time <= TSL2591_TIME_600MS; time++) {
            if (time == TSL2591_TIME_100MS) {
                measured_time[time] = 100.0F;
            } else {
                measured_time[time] = (ch0_readings[time] / ch0_readings[TSL2591_TIME_100MS]) * 100.0F;
            }

            float_to_str(measured_time[time], numbuf, 6);
            log_d("%dms -> %s", tsl2591_get_time_value_ms(time), numbuf);
            settings_set_cal_time(time, measured_time[time]);
        }
    } else {
        log_e("Integration time calibration failed");
    }

    return (ret == osOK) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef sensor_read(uint8_t iterations, float *ch0_result, float *ch1_result, sensor_read_callback_t callback, void *user_data)
{
    //TODO This is a test function to try and work out the new task-based process

    //TODO Should implement some sort of convergence algorithm instead of a fixed iteration count

    //TODO Could even provide continuous results via callback and do convergence code on the caller side

    osStatus_t ret = osOK;
    tsl2591_gain_t gain = TSL2591_GAIN_MAXIMUM;
    tsl2591_time_t time = SENSOR_DEFAULT_READ_TIME;
    float ch0_avg = NAN;
    float ch1_avg = NAN;
    bool is_saturated = false;

    log_i("Starting (task-based) read, %d iterations", iterations);

    /* Put the sensor into a known initial state, with maximum gain */
    sensor_set_config(gain, time);
    sensor_start();

    do {
        if (is_saturated) {
            if (gain == TSL2591_GAIN_LOW) { break; }
            gain--;

            /* Set the new gain value */
            sensor_set_config(gain, time);
            is_saturated = false;
        }

        /* Run the read loop */
        ret = sensor_read_loop(iterations, &ch0_avg, &ch1_avg, callback, user_data);
        if (ret != osOK) { break; }

        /* Check for saturation */
        if (isnanf(ch0_avg) || isnanf(ch1_avg)) {
            is_saturated = true;
        }
    } while (is_saturated && ret == osOK);

    /* Turn off the sensor */
    sensor_stop();

    if (!is_saturated && ret == osOK) {
        log_i("Sensor read complete");
        uint16_t time_ms = tsl2591_get_time_value_ms(time);
        log_d("TSL2591: CH0=%d, CH1=%d, Gain=[%d], Time=%dms", lroundf(ch0_avg), lroundf(ch1_avg), gain, time_ms);
        sensor_convert_to_basic_counts(gain, time, ch0_avg, ch1_avg, ch0_result, ch1_result);
    } else {
        log_e("Sensor read failed: is_saturated=%d, ret=%d", is_saturated, ret);
        if (ret == osOK) {
            ret = osError;
        }
    }
    return (ret == osOK) ? HAL_OK : HAL_ERROR;
}

/**
 * Sensor read loop.
 *
 * Assumes the sensor is already running and configured.
 *
 * @param count Number of values to average
 * @param ch0_avg Average reading of Channel 0
 * @param ch1_avg Average reading of Channel 1
 */
osStatus_t sensor_read_loop(uint8_t count, float *ch0_avg, float *ch1_avg,
    sensor_read_callback_t callback, void *user_data)
{
    osStatus_t ret = osOK;
    sensor_reading_t reading;
    float ch0_sum = 0;
    float ch1_sum = 0;
    bool saturation = false;

    if (count == 0) {
        return osErrorParameter;
    }

    /* Loop over measurements */
    for (uint8_t i = 0; i < count; i++) {
        /* Wait for the next reading */
        ret = sensor_get_next_reading(&reading, 2000);
        if (ret != osOK) {
            log_e("sensor_get_next_reading error: %d", ret);
            break;
        }

        /* Invoke the progress callback */
        if (callback) { callback(user_data); }

        /* Accumulate the results */
        log_v("TSL2591[%d]: CH0=%d, CH1=%d", i, reading.ch0_val, reading.ch1_val);
        if (sensor_is_reading_saturated(&reading)) {
            log_w("Sensor value indicates saturation");
            saturation = true;
            break;
        }
        ch0_sum += logf(reading.ch0_val);
        ch1_sum += logf(reading.ch1_val);
    }

    if (ret == osOK) {
        if (ch0_avg) {
            *ch0_avg = saturation ? NAN : expf(ch0_sum / (float)count);
        }
        if (ch1_avg) {
            *ch1_avg = saturation ? NAN : expf(ch1_sum / (float)count);
        }
    } else {
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
osStatus_t sensor_gain_calibration_loop(
    tsl2591_gain_t gain_low, tsl2591_gain_t gain_high, tsl2591_time_t time,
    float *gain_ch0, float *gain_ch1)
{
    osStatus_t ret = osOK;
    float ch0_avg_high;
    float ch1_avg_high;
    float ch0_avg_low;
    float ch1_avg_low;

    if (gain_low >= gain_high) {
        return HAL_ERROR;
    }

    do {
        /* Setup for high gain measurement */
        sensor_set_config(gain_high, time);

        /* Do the high gain read loop */
        log_d("Higher gain loop...");
        ret = sensor_read_loop(5, &ch0_avg_high, &ch1_avg_high, NULL, NULL);
        if (ret != osOK) { break; }

        log_d("TSL2591[Higher]: CH0=%d, CH1=%d", lroundf(ch0_avg_high), lroundf(ch1_avg_high));

        /* Setup for low gain measurement */
        sensor_set_config(gain_low, time);

        /* Do the low gain read loop */
        log_d("Lower gain loop...");
        ret = sensor_read_loop(5, &ch0_avg_low, &ch1_avg_low, NULL, NULL);
        if (ret != osOK) { break; }

        log_d("TSL2591[Lower]: CH0=%d, CH1=%d", lroundf(ch0_avg_low), lroundf(ch1_avg_low));
    } while (0);

    if (ret == osOK) {
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

bool sensor_is_reading_saturated(const sensor_reading_t *reading)
{
    if (!reading) {
        return false;
    }

    uint16_t limit;
    if (reading->time == TSL2591_TIME_100MS) {
        limit = TSL2591_ANALOG_SATURATION;
    } else {
        limit = TSL2591_DIGITAL_SATURATION;
    }
    if (reading->ch0_val >= limit || reading->ch1_val >= limit) {
        return true;
    } else {
        return false;
    }
}

void sensor_convert_to_basic_counts(tsl2591_gain_t gain, tsl2591_time_t time, float ch0_val, float ch1_val, float *ch0_basic, float *ch1_basic)
{
    float ch0_gain;
    float ch1_gain;
    float atime_ms;
    settings_get_cal_gain(gain, &ch0_gain, &ch1_gain);
    settings_get_cal_time(time, &atime_ms);

    float ch0_cpl = (atime_ms * ch0_gain) / (TSL2591_LUX_GA * TSL2591_LUX_DF);
    float ch1_cpl = (atime_ms * ch1_gain) / (TSL2591_LUX_GA * TSL2591_LUX_DF);

    if (ch0_basic) {
        *ch0_basic = ch0_val / ch0_cpl;
    }
    if (ch1_basic) {
        *ch1_basic = ch1_val / ch1_cpl;
    }
}
