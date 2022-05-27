#include "task_sensor.h"

#define LOG_TAG "task_sensor"
#include <elog.h>

#include <math.h>
#include <cmsis_os.h>
#include <FreeRTOS.h>
#include <queue.h>

#include "stm32l0xx_hal.h"
#include "settings.h"
#include "tsl2591.h"
#include "sensor.h"
#include "light.h"
#include "util.h"
#include "cdc_handler.h"

/**
 * Sensor control event types.
 */
typedef enum {
    SENSOR_CONTROL_STOP = 0,
    SENSOR_CONTROL_START,
    SENSOR_CONTROL_SET_CONFIG,
    SENSOR_CONTROL_SET_LIGHT_MODE,
    SENSOR_CONTROL_INTERRUPT
} sensor_control_event_type_t;

typedef struct {
    tsl2591_gain_t gain;
    tsl2591_time_t time;
} sensor_control_config_params_t;

typedef struct {
    sensor_light_t light;
    bool next_cycle;
    uint8_t value;
} sensor_control_light_mode_params_t;

typedef struct {
    uint32_t sensor_ticks;
    uint32_t light_ticks;
    uint32_t reading_count;
} sensor_control_interrupt_params_t;

/**
 * Sensor control event data.
 */
typedef struct {
    sensor_control_event_type_t event_type;
    osStatus_t *result;
    union {
        sensor_control_config_params_t config;
        sensor_control_light_mode_params_t light_mode;
        sensor_control_interrupt_params_t interrupt;
    };
} sensor_control_event_t;

/* Global I2C handle for the sensor */
extern I2C_HandleTypeDef hi2c1;

static volatile bool sensor_initialized = false;
static volatile uint32_t pending_int_light_change = 0;
static volatile uint32_t light_change_ticks = 0;
static volatile uint32_t reading_count = 0;

/* Sensor task state variables */
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

/* Sensor control implementation functions */
static osStatus_t sensor_control_start();
static osStatus_t sensor_control_stop();
static osStatus_t sensor_control_set_config(const sensor_control_config_params_t *params);
static osStatus_t sensor_control_set_light_mode(const sensor_control_light_mode_params_t *params);
static osStatus_t sensor_control_interrupt(const sensor_control_interrupt_params_t *params);

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
        sensor_initialized = false;
    } else {
        /* Set the initialized flag */
        sensor_initialized = true;
    }

    /* Release the startup semaphore */
    if (osSemaphoreRelease(task_start_semaphore) != osOK) {
        log_e("Unable to release task_start_semaphore");
        return;
    }

    /* Start the main control event loop */
    for (;;) {
        if(osMessageQueueGet(sensor_control_queue, &control_event, NULL, portMAX_DELAY) == osOK) {
            osStatus_t ret = osOK;
            switch (control_event.event_type) {
            case SENSOR_CONTROL_STOP:
                ret = sensor_control_stop();
                break;
            case SENSOR_CONTROL_START:
                ret = sensor_control_start();
                break;
            case SENSOR_CONTROL_SET_CONFIG:
                ret = sensor_control_set_config(&control_event.config);
                break;
            case SENSOR_CONTROL_SET_LIGHT_MODE:
                ret = sensor_control_set_light_mode(&control_event.light_mode);
                break;
            case SENSOR_CONTROL_INTERRUPT:
                ret = sensor_control_interrupt(&control_event.interrupt);
                break;
            default:
                break;
            }

            /* Handle all external commands by propagating their completion */
            if (control_event.event_type != SENSOR_CONTROL_INTERRUPT) {
                if (control_event.result) {
                    *(control_event.result) = ret;
                }
                if (osSemaphoreRelease(sensor_control_semaphore) != osOK) {
                    log_e("Unable to release sensor_control_semaphore");
                }
            }
        }
    }
}

bool sensor_is_initialized()
{
    return sensor_initialized;
}

osStatus_t sensor_start()
{
    if (!sensor_initialized) { return osErrorResource; }

    osStatus_t result = osOK;
    sensor_control_event_t control_event = {
        .event_type = SENSOR_CONTROL_START,
        .result = &result
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, portMAX_DELAY);
    osSemaphoreAcquire(sensor_control_semaphore, portMAX_DELAY);
    return result;
}

osStatus_t sensor_control_start()
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
        reading_count = 0;

        /* Enable ALS with interrupts */
        ret = tsl2591_set_enable(&hi2c1, TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN | TSL2591_ENABLE_AIEN);
        if (ret != HAL_OK) { break; }

        sensor_discard_next_reading = true;
        sensor_running = true;
    } while (0);

    return hal_to_os_status(ret);
}

osStatus_t sensor_stop()
{
    if (!sensor_initialized) { return osErrorResource; }

    osStatus_t result = osOK;
    sensor_control_event_t control_event = {
        .event_type = SENSOR_CONTROL_STOP,
        .result = &result
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, portMAX_DELAY);
    osSemaphoreAcquire(sensor_control_semaphore, portMAX_DELAY);
    return result;
}

osStatus_t sensor_control_stop()
{
    HAL_StatusTypeDef ret = HAL_OK;
    log_d("sensor_control_stop");

    do {
        ret = tsl2591_set_enable(&hi2c1, 0x00);
        if (ret != HAL_OK) { break; }
        sensor_running = false;
    } while (0);

    return hal_to_os_status(ret);
}

osStatus_t sensor_set_config(tsl2591_gain_t gain, tsl2591_time_t time)
{
    if (!sensor_initialized) { return osErrorResource; }

    osStatus_t result = osOK;
    sensor_control_event_t control_event = {
        .event_type = SENSOR_CONTROL_SET_CONFIG,
        .result = &result,
        .config = {
            .gain = gain,
            .time = time
        }
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, portMAX_DELAY);
    osSemaphoreAcquire(sensor_control_semaphore, portMAX_DELAY);
    return result;
}

osStatus_t sensor_control_set_config(const sensor_control_config_params_t *params)
{
    HAL_StatusTypeDef ret = HAL_OK;
    log_d("sensor_control_set_config: %d, %d", params->gain, params->time);

    if (sensor_running) {
        ret = tsl2591_set_config(&hi2c1, params->gain, params->time);
        if (ret == HAL_OK) {
            sensor_gain = params->gain;
            sensor_time = params->time;
            sensor_discard_next_reading = true;
            osMessageQueueReset(sensor_reading_queue);
        }
    } else {
        sensor_gain = params->gain;
        sensor_time = params->time;
    }

    return hal_to_os_status(ret);
}

osStatus_t sensor_set_light_mode(sensor_light_t light, bool next_cycle, uint8_t value)
{
    if (!sensor_initialized) { return osErrorResource; }

    osStatus_t result = osOK;
    sensor_control_event_t control_event = {
        .event_type = SENSOR_CONTROL_SET_LIGHT_MODE,
        .result = &result,
        .light_mode = {
            .light = light,
            .next_cycle = next_cycle,
            .value = value
        }
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, portMAX_DELAY);
    osSemaphoreAcquire(sensor_control_semaphore, portMAX_DELAY);
    return result;
}

osStatus_t sensor_control_set_light_mode(const sensor_control_light_mode_params_t *params)
{
    //log_d("sensor_set_light_mode: %d, %d, %d", params->light, params->next_cycle, params->value);

    uint8_t pending_reflection;
    uint8_t pending_transmission;

    /* Convert the parameters into pending values for the LEDs */
    if (params->light == SENSOR_LIGHT_OFF || params->value == 0) {
        pending_reflection = 0;
        pending_transmission = 0;
    } else if (params->light == SENSOR_LIGHT_REFLECTION) {
        pending_reflection = params->value;
        pending_transmission = 0;
    } else if (params->light == SENSOR_LIGHT_TRANSMISSION) {
        pending_reflection = 0;
        pending_transmission = params->value;
    } else {
        pending_reflection = 0;
        pending_transmission = 0;
    }

    taskENTER_CRITICAL();
    if (params->next_cycle) {
        /* Schedule the change for the next ISR invocation */
        pending_int_light_change = 0x80000000 | pending_reflection | (pending_transmission << 8);
    } else {
        /* Apply the change immediately */
        light_set_reflection(pending_reflection);
        light_set_transmission(pending_transmission);
        light_change_ticks = osKernelGetTickCount();
        pending_int_light_change = 0;
    }
    taskEXIT_CRITICAL();

    return osOK;
}

osStatus_t sensor_get_next_reading(sensor_reading_t *reading, uint32_t timeout)
{
    if (!sensor_initialized) { return osErrorResource; }

    if (!reading) {
        return osErrorParameter;
    }

    return osMessageQueueGet(sensor_reading_queue, reading, NULL, timeout);
}

void sensor_int_handler()
{
    if (!sensor_initialized) { return; }

    sensor_control_event_t control_event = {
        .event_type = SENSOR_CONTROL_INTERRUPT,
        .interrupt = {
            .sensor_ticks = osKernelGetTickCount()
        }
    };

    /* Apply any pending light change values */
    UBaseType_t interrupt_status = taskENTER_CRITICAL_FROM_ISR();
    if ((pending_int_light_change & 0x80000000) == 0x80000000) {
        light_set_reflection(pending_int_light_change & 0x000000FF);
        light_set_transmission((pending_int_light_change & 0x0000FF00) >> 8);
        light_change_ticks = osKernelGetTickCount();
        pending_int_light_change = 0;
    }
    control_event.interrupt.light_ticks = light_change_ticks;
    control_event.interrupt.reading_count = ++reading_count;
    taskEXIT_CRITICAL_FROM_ISR(interrupt_status);

    osMessageQueuePut(sensor_control_queue, &control_event, 0, 0);
}

osStatus_t sensor_control_interrupt(const sensor_control_interrupt_params_t *params)
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
        reading.reading_ticks = params->sensor_ticks;
        reading.light_ticks = params->light_ticks;
        reading.reading_count = params->reading_count;

        has_channel_data = true;
    } while (0);

    if (has_channel_data) {
        log_d("TSL2591[%d]: CH0=%d, CH1=%d, Gain=[%d], Time=%dms",
            reading.reading_count,
            reading.ch0_val, reading.ch1_val,
            sensor_gain, tsl2591_get_time_value_ms(sensor_time));

        cdc_send_raw_sensor_reading(&reading);

        QueueHandle_t queue = (QueueHandle_t)sensor_reading_queue;
        xQueueOverwrite(queue, &reading);
    }

    return hal_to_os_status(ret);
}
