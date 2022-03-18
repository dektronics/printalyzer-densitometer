#include "state_display.h"

#define LOG_TAG "state_display"

#include <stdbool.h>
#include <math.h>
#include <elog.h>

#include "keypad.h"
#include "display.h"
#include "light.h"
#include "task_sensor.h"
#include "densitometer.h"

typedef struct {
    state_t base;
    bool display_dirty;
    bool light_dirty;
    bool is_detect_prev;
    bool menu_pending;
    int up_repeat;
    int down_repeat;
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
    .light_dirty = true,
    .is_detect_prev = false,
    .menu_pending = false,
    .up_repeat = 0,
    .down_repeat = 0
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
    .light_dirty = true,
    .is_detect_prev = false,
    .menu_pending = false,
    .up_repeat = 0,
    .down_repeat = 0
};

state_t *state_reflection_display()
{
    return (state_t *)&state_reflection_display_data;
}

void state_reflection_display_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_display_t *state = (state_display_t *)state_base;
    state->display_dirty = true;
    state->light_dirty = true;
    state->menu_pending = false;
    state->up_repeat = 0;
    state->down_repeat = 0;
    state_controller_set_home_state(controller, STATE_REFLECTION_DISPLAY);
}

void state_reflection_display_process(state_t *state_base, state_controller_t *controller)
{
    state_display_t *state = (state_display_t *)state_base;
    bool is_detect = keypad_is_detect();
    if (is_detect != state->is_detect_prev) {
        state->light_dirty = true;
        state->is_detect_prev = is_detect;
    }

    if (state->light_dirty) {
        if (is_detect) {
            sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, LIGHT_REFLECTION_IDLE);
        } else {
            sensor_set_light_mode(SENSOR_LIGHT_OFF, false, 0);
        }
        state->light_dirty = false;
    }

    keypad_event_t keypad_event;
    if (keypad_wait_for_event(&keypad_event, STATE_KEYPAD_WAIT) == osOK) {
        state->display_dirty = true;

        if (state->menu_pending) {
            if ((keypad_event.keypad_state & (KEYPAD_BUTTON_UP | KEYPAD_BUTTON_DOWN)) == 0) {
                state->menu_pending = false;
                state_controller_set_next_state(controller, STATE_MAIN_MENU);
            }
        } else {
            if (is_detect && keypad_is_only_key_pressed(&keypad_event, KEYPAD_BUTTON_ACTION)) {
                state_controller_set_next_state(controller, STATE_REFLECTION_MEASURE);
            } else if (keypad_event.key == KEYPAD_BUTTON_UP) {
                if (keypad_event.pressed || keypad_event.repeated) {
                    if (state->up_repeat < 5) {
                        state->up_repeat++;
                    } else if (state->up_repeat == 5) {
                        log_i("Clear zero reflection measurement");
                        densitometer_reflection_clear_zero();
                        state->up_repeat++;
                        state->display_dirty = true;
                    }
                } else {
                    state->up_repeat = 0;
                    state->display_dirty = true;
                }
            } else if (keypad_event.key == KEYPAD_BUTTON_DOWN) {
                if (keypad_event.pressed || keypad_event.repeated) {
                    if (state->down_repeat < 5) {
                        state->down_repeat++;
                    } else if (state->down_repeat == 5) {
                        log_i("Setting zero reflection measurement");
                        densitometer_reflection_set_zero();
                        state->down_repeat++;
                        state->display_dirty = true;
                    }
                } else {
                    state->down_repeat = 0;
                    state->display_dirty = true;
                }
            } else if (keypad_is_only_key_pressed(&keypad_event, KEYPAD_BUTTON_MENU)) {
                state_controller_set_next_state(controller, STATE_TRANSMISSION_DISPLAY);
            }

            if (keypad_is_key_combo_pressed(&keypad_event, KEYPAD_BUTTON_UP, KEYPAD_BUTTON_DOWN)) {
                state->menu_pending = true;
                state->up_repeat = 0;
                state->down_repeat = 0;
            }
        }
    }

    if (state->display_dirty) {
        float reading = densitometer_reflection_get_last_reading();
        display_main_elements_t elements = {
            .title = "Reflection",
            .mode = DISPLAY_MODE_REFLECTION,
            .density100 = ((!isnanf(reading)) ? lroundf(reading * 100) : 0),
            .zero_indicator = densitometer_reflection_has_zero()
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
    state->light_dirty = true;
    state->menu_pending = false;
    state->up_repeat = 0;
    state->down_repeat = 0;
    state_controller_set_home_state(controller, STATE_TRANSMISSION_DISPLAY);
}

void state_transmission_display_process(state_t *state_base, state_controller_t *controller)
{
    state_display_t *state = (state_display_t *)state_base;
    bool is_detect = keypad_is_detect();
    if (is_detect != state->is_detect_prev) {
        state->light_dirty = true;
        state->is_detect_prev = is_detect;
    }

    if (state->light_dirty) {
        if (is_detect) {
            sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, LIGHT_TRANSMISSION_IDLE);
        } else {
            sensor_set_light_mode(SENSOR_LIGHT_OFF, false, 0);
        }
        state->light_dirty = false;
    }

    keypad_event_t keypad_event;
    if (keypad_wait_for_event(&keypad_event, STATE_KEYPAD_WAIT) == osOK) {
        state->display_dirty = true;

        if (state->menu_pending) {
            if ((keypad_event.keypad_state & (KEYPAD_BUTTON_UP | KEYPAD_BUTTON_DOWN)) == 0) {
                state->menu_pending = false;
                state_controller_set_next_state(controller, STATE_MAIN_MENU);
            }
        } else {
            if (is_detect && keypad_is_only_key_pressed(&keypad_event, KEYPAD_BUTTON_ACTION)) {
                state_controller_set_next_state(controller, STATE_TRANSMISSION_MEASURE);
            } else if (keypad_event.key == KEYPAD_BUTTON_UP) {
                if (keypad_event.pressed || keypad_event.repeated) {
                    if (state->up_repeat < 5) {
                        state->up_repeat++;
                    } else if (state->up_repeat == 5) {
                        log_i("Clear zero transmission measurement");
                        densitometer_transmission_clear_zero();
                        state->up_repeat++;
                        state->display_dirty = true;
                    }
                } else {
                    state->up_repeat = 0;
                    state->display_dirty = true;
                }
            } else if (keypad_event.key == KEYPAD_BUTTON_DOWN) {
                if (keypad_event.pressed || keypad_event.repeated) {
                    if (state->down_repeat < 5) {
                        state->down_repeat++;
                    } else if (state->down_repeat == 5) {
                        log_i("Setting zero transmission measurement");
                        densitometer_transmission_set_zero();
                        state->down_repeat++;
                        state->display_dirty = true;
                    }
                } else {
                    state->down_repeat = 0;
                    state->display_dirty = true;
                }
            } else if (keypad_is_only_key_pressed(&keypad_event, KEYPAD_BUTTON_MENU)) {
                state_controller_set_next_state(controller, STATE_REFLECTION_DISPLAY);
            }

            if (keypad_is_key_combo_pressed(&keypad_event, KEYPAD_BUTTON_UP, KEYPAD_BUTTON_DOWN)) {
                state->menu_pending = true;
                state->up_repeat = 0;
                state->down_repeat = 0;
            }
        }
    }

    if (state->display_dirty) {
        float reading = densitometer_transmission_get_last_reading();
        display_main_elements_t elements = {
            .title = "Transmission",
            .mode = DISPLAY_MODE_TRANSMISSION,
            .density100 = ((!isnanf(reading)) ? lroundf(reading * 100) : 0),
            .zero_indicator = densitometer_transmission_has_zero()
        };
        display_draw_main_elements(&elements);
        state->display_dirty = false;
    }
}
