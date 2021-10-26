#include "keypad.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <cmsis_os.h>
#include <FreeRTOS.h>
#include <timers.h>

#ifndef KEYPAD_DEBUG
#define LOG_LVL  0
#endif
#define LOG_TAG "keypad"
#include <elog.h>

#include "stm32l0xx_hal.h"
#include "board_config.h"

#define KEYPAD_INDEX_MAX       5
#define KEYPAD_REPEAT_DELAY_MS 600
#define KEYPAD_REPEAT_RATE_S   25

/* Internal raw keypad event data */
typedef struct {
    keypad_key_t keycode;
    bool pressed;
    bool repeated;
    bool prev_dropped;
} keypad_raw_event_t;

/* Basic key state variables */
static volatile bool keypad_initialized = false;
static uint8_t keypad_raw_state = 0;

/* Currently known state of all keypad buttons */
static uint16_t button_state = 0;

/* Timer for button repeat events */
static TimerHandle_t button_repeat_timer = NULL;

/* Whether timer is in initial delay or repeating event mode */
static UBaseType_t button_repeat_timer_reload;

/* Queue for raw keypad events, which come from the controller or timers */
static osMessageQueueId_t keypad_raw_event_queue = NULL;
static const osMessageQueueAttr_t keypad_raw_event_queue_attrs = {
    .name = "keypad_raw_event_queue"
};

/* Queue for emitted keypad events, which are handled by the application */
static osMessageQueueId_t keypad_event_queue = NULL;
static const osMessageQueueAttr_t keypad_event_queue_attrs = {
    .name = "keypad_event_queue"
};

static void keypad_handle_key_event(uint8_t keycode, bool pressed);
static void keypad_handle_key_repeat(uint8_t keycode);
static void keypad_button_repeat_timer_callback(TimerHandle_t xTimer);
static uint8_t keypad_keycode_to_index(keypad_key_t keycode);
static bool keypad_keycode_can_repeat(keypad_key_t keycode);

void task_keypad_run(void *argument)
{
    osSemaphoreId_t task_start_semaphore = argument;
    keypad_raw_event_t raw_event;

    log_d("keypad_task start");

    /* Create the queues for key events */
    keypad_raw_event_queue = osMessageQueueNew(20, sizeof(keypad_raw_event_t), &keypad_raw_event_queue_attrs);
    if (!keypad_raw_event_queue) {
        log_e("Unable to create event queue");
        return;
    }

    keypad_event_queue = osMessageQueueNew(20, sizeof(keypad_event_t), &keypad_event_queue_attrs);
    if (!keypad_event_queue) {
        log_e("Unable to create event queue");
        return;
    }

    /* Clear the button states */
    keypad_raw_state = 0;

    /* Create the timer to handle key repeat events */
    button_repeat_timer = xTimerCreate(
        "keypad_repeat", 1, pdFALSE, (void *)0,
        keypad_button_repeat_timer_callback);
    if (!button_repeat_timer) {
        log_e("Unable to create repeat timer");
        return;
    }
    button_repeat_timer_reload = pdFALSE;

    /* Set the initialized flag */
    keypad_initialized = true;

    /* Release the startup semaphore */
    if (osSemaphoreRelease(task_start_semaphore) != osOK) {
        log_e("Unable to release task_start_semaphore");
        return;
    }

    for (;;) {
        if(osMessageQueueGet(keypad_raw_event_queue, &raw_event, NULL, portMAX_DELAY) == osOK) {
            if (raw_event.prev_dropped) {
                log_w("Raw key event missed!");
                /*
                 * The simplest way to recover from this is to cancel the
                 * repeat timer and zero out the known button state.
                 * Since this condition is only expected in cases of frantic
                 * button mashing, it is probably okay to miss events in a
                 * way that are unlikely to break the system.
                 */
                xTimerStop(button_repeat_timer, portMAX_DELAY);
                button_state = 0;
            }
            if (!raw_event.repeated) {
                keypad_handle_key_event(raw_event.keycode, raw_event.pressed);
            } else {
                keypad_handle_key_repeat(raw_event.keycode);
            }
        }
    }
}

osStatus_t keypad_inject_event(const keypad_event_t *event)
{
    if (!event) {
        return osErrorParameter;
    }

    return osMessageQueuePut(keypad_event_queue, event, 0, 0);
}

osStatus_t keypad_clear_events()
{
    return osMessageQueueReset(keypad_event_queue);
}

osStatus_t keypad_flush_events()
{
    keypad_event_t event;
    bzero(&event, sizeof(keypad_event_t));
    osMessageQueueReset(keypad_event_queue);
    return osMessageQueuePut(keypad_event_queue, &event, 0, 0);
}

osStatus_t keypad_wait_for_event(keypad_event_t *event, int msecs_to_wait)
{
    TickType_t ticks = msecs_to_wait < 0 ? portMAX_DELAY : (msecs_to_wait / portTICK_RATE_MS);

    if (osMessageQueueGet(keypad_event_queue, event, NULL, ticks) != osOK) {
        if (msecs_to_wait > 0) {
            return osErrorTimeout;
        } else {
            bzero(event, sizeof(keypad_event_t));
        }
    }
    return osOK;
}

bool keypad_is_key_pressed(const keypad_event_t *event, keypad_key_t key)
{
    if (!event) { return false; }

    int index = keypad_keycode_to_index(key);
    if (index < KEYPAD_INDEX_MAX && event && event->keypad_state & (1 << index)) {
        return true;
    } else {
        return false;
    }
}

bool keypad_is_only_key_pressed(const keypad_event_t *event, keypad_key_t key)
{
    if (!event) { return false; }

    int index = keypad_keycode_to_index(key);
    int detect_index = keypad_keycode_to_index(KEYPAD_BUTTON_DETECT);
    uint16_t masked_state = event->keypad_state & ~(1 << detect_index);
    if (index < KEYPAD_INDEX_MAX && event && masked_state == (1 << index)) {
        return true;
    } else {
        return false;
    }
}

bool keypad_is_key_released_or_repeated(const keypad_event_t *event, keypad_key_t key)
{
    if (!event) { return false; }

    if (event->key == key && (!event->pressed || event->repeated)) {
        return true;
    } else {
        return false;
    }
}

bool keypad_is_key_combo_pressed(const keypad_event_t *event, keypad_key_t key1, keypad_key_t key2)
{
    if (!event) { return false; }

    if (((event->key == key1 && keypad_is_key_pressed(event, key2))
        || (event->key == key2 && keypad_is_key_pressed(event, key1)))
        && event->pressed) {
        return true;
    } else {
        return false;
    }
}

void keypad_handle_key_event(uint8_t keycode, bool pressed)
{
    /* Update the button state information */
    TickType_t tick_duration = 0;
    uint8_t index = keypad_keycode_to_index(keycode);
    if (index < KEYPAD_INDEX_MAX) {
        uint16_t mask = 1 << index;
        if (pressed) {
            button_state |= mask;
        } else {
            button_state &= ~mask;
        }
    }

    /* Limit the max duration we can report */
    if (tick_duration > UINT16_MAX) {
        tick_duration = UINT16_MAX;
    }

    /* Handle keys that can repeat */
    if (keypad_keycode_can_repeat(keycode)) {
        if (pressed) {
            /* Pressing a repeatable key should reset and restart the repeat timer. */
            if (xTimerIsTimerActive(button_repeat_timer) == pdTRUE) {
                xTimerStop(button_repeat_timer, portMAX_DELAY);
            }

            uint32_t keycode_id = keycode;
            xTimerChangePeriod(button_repeat_timer, pdMS_TO_TICKS(KEYPAD_REPEAT_DELAY_MS), portMAX_DELAY);
            vTimerSetReloadMode(button_repeat_timer, pdFALSE);
            button_repeat_timer_reload = pdFALSE;
            vTimerSetTimerID(button_repeat_timer, (void *)keycode_id);
            xTimerStart(button_repeat_timer, portMAX_DELAY);
        } else {
            /* Releasing a repeatable key should stop the repeat timer */
            if (xTimerIsTimerActive(button_repeat_timer) == pdTRUE) {
                xTimerStop(button_repeat_timer, portMAX_DELAY);
            }
        }
    }

    /* Generate the keypad event */
    keypad_event_t keypad_event = {
        .key = keycode,
        .pressed = pressed,
        .repeated = false,
        .keypad_state = button_state
    };
    log_d("Key event: key=%d, pressed=%d, state=%04X", keycode, pressed, button_state);

    osMessageQueuePut(keypad_event_queue, &keypad_event, 0, 0);
}

void keypad_handle_key_repeat(uint8_t keycode)
{
    /* Make sure the repeated key is still pressed, and shortcut out if it is not. */
    int index = keypad_keycode_to_index(keycode);
    if (index < KEYPAD_INDEX_MAX && !(button_state & (1 << index))) {
        xTimerStop(button_repeat_timer, portMAX_DELAY);
        return;
    }

    /* Generate the keypad event */
    keypad_event_t keypad_event = {
        .key = keycode,
        .pressed = true,
        .repeated = true,
        .keypad_state = button_state
    };
    log_d("Key event: key=%d, pressed=1, state=%04X (repeat)", keycode, button_state);

    if (osMessageQueuePut(keypad_event_queue, &keypad_event, 0, 0) == osOK) {
        if (button_repeat_timer_reload == pdFALSE) {
            /* The initial period elapsed, so reconfigure for key repeat */
            xTimerStop(button_repeat_timer, portMAX_DELAY);
            xTimerChangePeriod(button_repeat_timer, pdMS_TO_TICKS(1000) / KEYPAD_REPEAT_RATE_S, portMAX_DELAY);
            vTimerSetReloadMode(button_repeat_timer, pdTRUE);
            button_repeat_timer_reload = pdTRUE;
            xTimerStart(button_repeat_timer, portMAX_DELAY);
        }
    } else {
        log_w("Skipping key repeat due to raw queue overflow");
    }
}

void keypad_button_repeat_timer_callback(TimerHandle_t xTimer)
{
    uint32_t keycode_id = (uint32_t)pvTimerGetTimerID(xTimer);

    keypad_raw_event_t raw_event = {
        .keycode = (uint16_t)keycode_id,
        .pressed = true,
        .repeated = true
    };
    osMessageQueuePut(keypad_raw_event_queue, &raw_event, 0, 0);
}

bool keypad_is_detect()
{
    /*
     * This key is routed through the normal key event queue to prevent
     * tasks that block on that queue from ignoring immediate changes
     * in its state. However, since its immediate state is more relevant
     * than a change event, it should normally be sampled by directly
     * polling the GPIO pin.
     */
    return HAL_GPIO_ReadPin(BTN5_GPIO_Port, BTN5_Pin) == GPIO_PIN_RESET;
}

uint8_t keypad_keycode_to_index(keypad_key_t keycode)
{
    switch (keycode) {
    case KEYPAD_BUTTON_ACTION:
        return 0;
    case KEYPAD_BUTTON_UP:
        return 1;
    case KEYPAD_BUTTON_DOWN:
        return 2;
    case KEYPAD_BUTTON_MENU:
        return 3;
    case KEYPAD_BUTTON_DETECT:
        return 4;
    default:
        return KEYPAD_INDEX_MAX;
    }
}

bool keypad_keycode_can_repeat(keypad_key_t keycode)
{
    switch (keycode) {
    case KEYPAD_BUTTON_UP:
    case KEYPAD_BUTTON_DOWN:
        return true;
    case KEYPAD_BUTTON_ACTION:
    case KEYPAD_BUTTON_MENU:
    case KEYPAD_BUTTON_DETECT:
    default:
        return false;
    }
}

void keypad_int_handler(uint16_t gpio_pin)
{
    static bool raw_event_dropped = false;
    GPIO_TypeDef *gpio_port = 0;
    GPIO_PinState button_curr_state = 0;
    GPIO_PinState button_last_state = 0;
    uint8_t button_mask = 0;

    if (!keypad_initialized) { return; }

    switch (gpio_pin) {
    case BTN1_Pin:
        gpio_port = BTN1_GPIO_Port;
        button_mask = KEYPAD_BUTTON_ACTION;
        break;
    case BTN2_Pin:
        gpio_port = BTN2_GPIO_Port;
        button_mask = KEYPAD_BUTTON_UP;
        break;
    case BTN3_Pin:
        gpio_port = BTN3_GPIO_Port;
        button_mask = KEYPAD_BUTTON_DOWN;
        break;
    case BTN4_Pin:
        gpio_port = BTN4_GPIO_Port;
        button_mask = KEYPAD_BUTTON_MENU;
        break;
    case BTN5_Pin:
        gpio_port = BTN5_GPIO_Port;
        button_mask = KEYPAD_BUTTON_DETECT;
        break;
    default:
        return;
    }

    button_curr_state = HAL_GPIO_ReadPin(gpio_port, gpio_pin);
    button_last_state = (keypad_raw_state & button_mask) ? GPIO_PIN_SET : GPIO_PIN_RESET;

    if (button_curr_state != button_last_state || gpio_pin == BTN5_Pin) {
        if (button_curr_state == GPIO_PIN_SET) {
            keypad_raw_state |= button_mask;
        } else {
            keypad_raw_state &= ~(button_mask);
        }

        keypad_raw_event_t raw_event = {
            .keycode = button_mask,
            .pressed = button_curr_state,
            .repeated = false,
            .prev_dropped = raw_event_dropped
        };
        if (osMessageQueuePut(keypad_raw_event_queue, &raw_event, 0, 0) == osOK) {
            raw_event_dropped = false;
        } else {
            raw_event_dropped = true;
        }
    }
}
