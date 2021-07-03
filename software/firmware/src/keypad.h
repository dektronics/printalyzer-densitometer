#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>

typedef enum {
    KEYPAD_BUTTON_1 = 0x01,
    KEYPAD_BUTTON_2 = 0x02,
    KEYPAD_BUTTON_3 = 0x04,
    KEYPAD_BUTTON_4 = 0x08,
    KEYPAD_BUTTON_5 = 0x10
} keypad_key_t;

uint8_t keypad_get_state();

void keypad_int_handler(uint16_t gpio_pin);

#endif /* KEYPAD_H */
