#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>
#include <stdbool.h>
#include <cmsis_os.h>

/* #define KEYPAD_DEBUG */

typedef enum {
    KEYPAD_BUTTON_ACTION = 0x01,
    KEYPAD_BUTTON_UP = 0x02,
    KEYPAD_BUTTON_DOWN = 0x04,
    KEYPAD_BUTTON_MENU = 0x08,
    KEYPAD_BUTTON_DETECT = 0x10
} keypad_key_t;

typedef struct {
    keypad_key_t key;
    bool pressed;
    bool repeated;
    uint16_t keypad_state;
} keypad_event_t;

void task_keypad_run(void *argument);

osStatus_t keypad_inject_event(const keypad_event_t *event);
osStatus_t keypad_clear_events();
osStatus_t keypad_flush_events();
osStatus_t keypad_wait_for_event(keypad_event_t *event, int msecs_to_wait);

bool keypad_is_key_pressed(const keypad_event_t *event, keypad_key_t key);
bool keypad_is_key_released_or_repeated(const keypad_event_t *event, keypad_key_t key);
bool keypad_is_key_combo_pressed(const keypad_event_t *event, keypad_key_t key1, keypad_key_t key2);

bool keypad_is_detect();

void keypad_int_handler(uint16_t gpio_pin);

#endif /* KEYPAD_H */
