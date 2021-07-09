#include "state_display.h"

#define LOG_TAG "state_display"

#include <stdbool.h>
#include <math.h>
#include <elog.h>

#include "keypad.h"
#include "display.h"
#include "light.h"
#include "densitometer.h"

typedef struct {
    state_t base;
    bool display_dirty;
    bool menu_pending;
} state_display_t;

static void state_reflection_display_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_reflection_display_process(state_t *state_base, state_controller_t *controller);
static state_display_t state_reflection_display_data = {
    .base = {
        .state_entry = state_reflection_display_entry,
        .state_process = state_reflection_display_process,
        .state_exit = NULL
    },
    .display_dirty = true,
    .menu_pending = false
};

static void state_transmission_display_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_transmission_display_process(state_t *state_base, state_controller_t *controller);
static state_display_t state_transmission_display_data = {
    .base = {
        .state_entry = state_transmission_display_entry,
        .state_process = state_transmission_display_process,
        .state_exit = NULL
    },
    .display_dirty = true,
    .menu_pending = false
};

state_t *state_reflection_display()
{
    return (state_t *)&state_reflection_display_data;
}

void state_reflection_display_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_display_t *state = (state_display_t *)state_base;
    state->display_dirty = true;
    state->menu_pending = false;

    light_set_reflection(LIGHT_REFLECTION_IDLE);
    light_set_transmission(0);
}

void state_reflection_display_process(state_t *state_base, state_controller_t *controller)
{
    state_display_t *state = (state_display_t *)state_base;
    uint8_t key_state = keypad_get_state();

    if (key_state != 0xFF) {
        state->display_dirty = true;

        if (state->menu_pending) {
            if ((key_state & (KEYPAD_BUTTON_2 | KEYPAD_BUTTON_3)) == 0) {
                state->menu_pending = false;
                state_controller_set_next_state(controller, STATE_MAIN_MENU);
            }
        } else {
            if (key_state & KEYPAD_BUTTON_1) {
                state_controller_set_next_state(controller, STATE_REFLECTION_MEASURE);
            } else if ((key_state & KEYPAD_BUTTON_2) && (key_state & KEYPAD_BUTTON_3)) {
                state->menu_pending = true;
            } else if (key_state & KEYPAD_BUTTON_4) {
                state_controller_set_next_state(controller, STATE_TRANSMISSION_DISPLAY);
            }
        }
    }

    if (state->display_dirty) {
        float reading = densitometer_reflection_get_last_reading();
        display_main_elements_t elements = {
            .mode = DISPLAY_MODE_REFLECTION,
            .density100 = (!isnanf(reading)) ? lroundf(reading * 100) : 0
        };
        display_draw_main_elements(&elements);
        state->display_dirty = false;
    }
}

state_t *state_transmission_display()
{
    return (state_t *)&state_transmission_display_data;
}

void state_transmission_display_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_display_t *state = (state_display_t *)state_base;
    state->display_dirty = true;
    state->menu_pending = false;

    light_set_reflection(0);
    light_set_transmission(LIGHT_TRANSMISSION_IDLE);
}

void state_transmission_display_process(state_t *state_base, state_controller_t *controller)
{
    state_display_t *state = (state_display_t *)state_base;
    uint8_t key_state = keypad_get_state();

    if (key_state != 0xFF) {
        state->display_dirty = true;

        if (state->menu_pending) {
            if ((key_state & (KEYPAD_BUTTON_2 | KEYPAD_BUTTON_3)) == 0) {
                state->menu_pending = false;
                state_controller_set_next_state(controller, STATE_MAIN_MENU);
            }
        } else {
            if (key_state & KEYPAD_BUTTON_1) {
                state_controller_set_next_state(controller, STATE_TRANSMISSION_MEASURE);
            } else if ((key_state & KEYPAD_BUTTON_2) && (key_state & KEYPAD_BUTTON_3)) {
                state->menu_pending = true;
            } else if (key_state & KEYPAD_BUTTON_4) {
                state_controller_set_next_state(controller, STATE_REFLECTION_DISPLAY);
            }
        }
    }

    if (state->display_dirty) {
        float reading = densitometer_transmission_get_last_reading();
        display_main_elements_t elements = {
            .mode = DISPLAY_MODE_TRANSMISSION,
            .density100 = (!isnanf(reading)) ? lroundf(reading * 100) : 0
        };
        display_draw_main_elements(&elements);
        state->display_dirty = false;
    }
}
