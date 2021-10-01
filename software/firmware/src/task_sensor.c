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

/**
 * Sensor control event data.
 */
typedef struct {
    uint32_t ticks;
    sensor_control_event_type_t event_type;
    union {
        struct {
            uint8_t param1;
            uint8_t param2;
            uint8_t param3;
            uint8_t param4;
        } b;
        uint32_t param;
    };
} sensor_control_event_t;

/* Global I2C handle for the sensor */
extern I2C_HandleTypeDef hi2c1;

static volatile bool sensor_initialized = false;
static volatile uint32_t pending_int_light_change = 0;
static volatile uint32_t light_change_ticks = 0;

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
static void sensor_control_start();
static void sensor_control_stop();
static void sensor_control_set_config(tsl2591_gain_t gain, tsl2591_time_t time);
static void sensor_control_set_light_mode(sensor_light_t light, bool next_cycle, uint8_t value);
static void sensor_control_interrupt(uint32_t sensor_ticks, uint32_t light_ticks);

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
                sensor_control_set_config((tsl2591_gain_t)control_event.b.param1, (tsl2591_time_t)control_event.b.param2);
                break;
            case SENSOR_CONTROL_SET_LIGHT_MODE:
                sensor_control_set_light_mode(
                    (sensor_light_t)control_event.b.param1,
                    (bool)control_event.b.param2,
                    control_event.b.param3);
                break;
            case SENSOR_CONTROL_INTERRUPT:
                sensor_control_interrupt(control_event.ticks, control_event.param);
                break;
            default:
                break;
            }
        }
    }
}

bool sensor_is_initialized()
{
    return sensor_initialized;
}

void sensor_start()
{
    sensor_control_event_t control_event = {
        .ticks = osKernelGetTickCount(),
        .event_type = SENSOR_CONTROL_START
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, portMAX_DELAY);

    if (osSemaphoreAcquire(sensor_control_semaphore, portMAX_DELAY) != osOK) {
        log_e("Unable to acquire sensor_control_semaphore");
    }
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

void sensor_stop()
{
    sensor_control_event_t control_event = {
        .ticks = osKernelGetTickCount(),
        .event_type = SENSOR_CONTROL_STOP
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, portMAX_DELAY);

    if (osSemaphoreAcquire(sensor_control_semaphore, portMAX_DELAY) != osOK) {
        log_e("Unable to acquire sensor_control_semaphore");
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

void sensor_set_config(tsl2591_gain_t gain, tsl2591_time_t time)
{
    sensor_control_event_t control_event = {
        .ticks = osKernelGetTickCount(),
        .event_type = SENSOR_CONTROL_SET_CONFIG,
        .b = {
            .param1 = gain,
            .param2 = time
        }
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, portMAX_DELAY);

    if (osSemaphoreAcquire(sensor_control_semaphore, portMAX_DELAY) != osOK) {
        log_e("Unable to acquire sensor_control_semaphore");
    }
}

void sensor_control_set_config(tsl2591_gain_t gain, tsl2591_time_t time)
{
    log_d("sensor_control_set_config: %d, %d", gain, time);

    if (sensor_running) {
        if (tsl2591_set_config(&hi2c1, gain, time) == HAL_OK) {
            sensor_gain = gain;
            sensor_time = time;
            sensor_discard_next_reading = true;
            osMessageQueueReset(sensor_reading_queue);
        }
    } else {
        sensor_gain = gain;
        sensor_time = time;
    }

    if (osSemaphoreRelease(sensor_control_semaphore) != osOK) {
        log_e("Unable to release sensor_control_semaphore");
    }
}

void sensor_set_light_mode(sensor_light_t light, bool next_cycle, uint8_t value)
{
    //TODO have a way to ignore commands that don't change the current state
    sensor_control_event_t control_event = {
        .ticks = osKernelGetTickCount(),
        .event_type = SENSOR_CONTROL_SET_LIGHT_MODE,
        .b = {
            .param1 = light,
            .param2 = next_cycle,
            .param3 = value
        }
    };
    osMessageQueuePut(sensor_control_queue, &control_event, 0, portMAX_DELAY);

    if (osSemaphoreAcquire(sensor_control_semaphore, portMAX_DELAY) != osOK) {
        log_e("Unable to acquire sensor_control_semaphore");
    }
}

void sensor_control_set_light_mode(sensor_light_t light, bool next_cycle, uint8_t value)
{
    //log_d("sensor_set_light_mode: %d, %d, %d", light, next_cycle, value);

    //TODO Handle tracking of on-time/off-time
    //TODO Implement next_cycle behavior

    uint8_t pending_reflection;
    uint8_t pending_transmission;

    /* Convert the parameters into pending values for the LEDs */
    if (light == SENSOR_LIGHT_OFF || value == 0) {
        pending_reflection = 0;
        pending_transmission = 0;
    } else if (light == SENSOR_LIGHT_REFLECTION) {
        pending_reflection = value;
        pending_transmission = 0;
    } else if (light == SENSOR_LIGHT_TRANSMISSION) {
        pending_reflection = 0;
        pending_transmission = value;
    } else {
        pending_reflection = 0;
        pending_transmission = 0;
    }

    taskENTER_CRITICAL();
    if (next_cycle) {
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

    if (osSemaphoreRelease(sensor_control_semaphore) != osOK) {
        log_e("Unable to release sensor_control_semaphore");
    }
}

osStatus_t sensor_get_next_reading(sensor_reading_t *reading, uint32_t timeout)
{
    if (!reading) {
        return osErrorParameter;
    }

    return osMessageQueueGet(sensor_reading_queue, reading, NULL, timeout);
}

void sensor_int_handler()
{
    sensor_control_event_t control_event = {
        .ticks = osKernelGetTickCount(),
        .event_type = SENSOR_CONTROL_INTERRUPT
    };

    /* Apply any pending light change values */
    UBaseType_t interrupt_status = taskENTER_CRITICAL_FROM_ISR();
    if ((pending_int_light_change & 0x80000000) == 0x80000000) {
        light_set_reflection(pending_int_light_change & 0x000000FF);
        light_set_transmission((pending_int_light_change & 0x0000FF00) >> 8);
        light_change_ticks = osKernelGetTickCount();
        control_event.param = light_change_ticks;
    }
    taskEXIT_CRITICAL_FROM_ISR(interrupt_status);

    osMessageQueuePut(sensor_control_queue, &control_event, 0, 0);
}

void sensor_control_interrupt(uint32_t sensor_ticks, uint32_t light_ticks)
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
        reading.reading_ticks = sensor_ticks;
        reading.light_ticks = light_ticks;

        has_channel_data = true;
    } while (0);

    //TODO need to make sure we can't jam in some of the failure cases

    if (has_channel_data) {
        log_d("TSL2591: CH0=%d, CH1=%d, Gain=[%d], Time=%dms", reading.ch0_val, reading.ch1_val, sensor_gain, tsl2591_get_time_value_ms(sensor_time));

        QueueHandle_t queue = (QueueHandle_t)sensor_reading_queue;
        xQueueOverwrite(queue, &reading);
    }
}
