#include "keypad.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

#include "stm32l0xx_hal.h"
#include "board_config.h"

static uint8_t keypad_state = 0;
static bool keypad_state_changed = false;

//TODO Need to rewrite this to use some sort of key event queue

uint8_t keypad_get_state()
{
    uint8_t result = 0;

    HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
    HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);

    if (keypad_state_changed) {
        result = keypad_state;
        keypad_state_changed = false;
    } else {
        result = 0xFF;
    }

    HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

    return result;
}

bool keypad_is_detect()
{
    return HAL_GPIO_ReadPin(BTN5_GPIO_Port, BTN5_Pin) == GPIO_PIN_RESET;
}

void keypad_int_handler(uint16_t gpio_pin)
{
    GPIO_TypeDef *gpio_port = 0;
    GPIO_PinState button_curr_state = 0;
    GPIO_PinState button_last_state = 0;
    uint8_t button_mask = 0;

    switch (gpio_pin) {
    case BTN1_Pin:
        gpio_port = BTN1_GPIO_Port;
        button_mask = KEYPAD_BUTTON_1;
        break;
    case BTN2_Pin:
        gpio_port = BTN2_GPIO_Port;
        button_mask = KEYPAD_BUTTON_2;
        break;
    case BTN3_Pin:
        gpio_port = BTN3_GPIO_Port;
        button_mask = KEYPAD_BUTTON_3;
        break;
    case BTN4_Pin:
        gpio_port = BTN4_GPIO_Port;
        button_mask = KEYPAD_BUTTON_4;
        break;
    default:
        return;
    }

    button_curr_state = HAL_GPIO_ReadPin(gpio_port, gpio_pin);
    button_last_state = (keypad_state & button_mask) ? GPIO_PIN_SET : GPIO_PIN_RESET;

    if (button_curr_state != button_last_state) {
        if (button_curr_state == GPIO_PIN_SET) {
            keypad_state |= button_mask;
        } else {
            keypad_state &= ~(button_mask);
        }
        keypad_state_changed = true;
    }
}
