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
    state_identifier_t measure_state;
    state_identifier_t alternate_state;
    densitometer_t *densitometer;
    const char *display_title;
    display_mode_t display_mode;
} state_display_t;

static void state_display_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_reflection_display_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_transmission_display_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);

static void state_display_process(state_t *state_base, state_controller_t *controller);

static state_display_t state_reflection_display_data = {
    .base = {
        .state_entry = state_reflection_display_entry,
        .state_process = state_display_process,
        .state_exit = NULL
    },
    .display_dirty = true,
    .light_dirty = true,
    .is_detect_prev = false,
    .menu_pending = false,
    .up_repeat = 0,
    .down_repeat = 0,
    .measure_state = STATE_REFLECTION_MEASURE,
    .alternate_state = STATE_TRANSMISSION_DISPLAY,
    .densitometer = NULL,
    .display_title = "Reflection",
    .display_mode = DISPLAY_MODE_REFLECTION
};

static state_display_t state_transmission_display_data = {
    .base = {
        .state_entry = state_transmission_display_entry,
        .state_process = state_display_process,
        .state_exit = NULL
    },
    .display_dirty = true,
    .light_dirty = true,
    .is_detect_prev = false,
    .menu_pending = false,
    .up_repeat = 0,
    .down_repeat = 0,
    .measure_state = STATE_TRANSMISSION_MEASURE,
    .alternate_state = STATE_REFLECTION_DISPLAY,
    .densitometer = NULL,
    .display_title = "Transmission",
    .display_mode = DISPLAY_MODE_TRANSMISSION
};

state_t *state_reflection_display()
{
    return (state_t *)&state_reflection_display_data;
}

state_t *state_transmission_display()
{
    return (state_t *)&state_transmission_display_data;
}

void state_display_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_display_t *state = (state_display_t *)state_base;

    state->display_dirty = true;
    state->light_dirty = true;
    state->menu_pending = false;
    state->up_repeat = 0;
    state->down_repeat = 0;
    state_controller_set_home_state(controller, state_controller_get_current_state(controller));
}

void state_reflection_display_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_display_entry(state_base, controller, prev_state);

    state_display_t *state = (state_display_t *)state_base;
    state->densitometer = densitometer_reflection();
}

void state_transmission_display_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_display_entry(state_base, controller, prev_state);

    state_display_t *state = (state_display_t *)state_base;
    state->densitometer = densitometer_transmission();
}

void state_display_process(state_t *state_base, state_controller_t *controller)
{
    state_display_t *state = (state_display_t *)state_base;
    bool is_detect = keypad_is_detect();
    if (is_detect != state->is_detect_prev) {
        state->light_dirty = true;
        state->is_detect_prev = is_detect;
    }

    if (state->light_dirty) {
        densitometer_set_idle_light(state->densitometer, is_detect);
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
                state_controller_set_next_state(controller, state->measure_state);
            } else if (keypad_event.key == KEYPAD_BUTTON_UP) {
                if (keypad_event.pressed || keypad_event.repeated) {
                    if (state->up_repeat < 5) {
                        state->up_repeat++;
                    } else if (state->up_repeat == 5) {
                        log_i("Clear zero measurement");
                        densitometer_clear_zero(state->densitometer);
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
                        log_i("Setting zero measurement");
                        densitometer_set_zero(state->densitometer);
                        state->down_repeat++;
                        state->display_dirty = true;
                    }
                } else {
                    state->down_repeat = 0;
                    state->display_dirty = true;
                }
            } else if (keypad_is_only_key_pressed(&keypad_event, KEYPAD_BUTTON_MENU)) {
                state_controller_set_next_state(controller, state->alternate_state);
            }

            if (keypad_is_key_combo_pressed(&keypad_event, KEYPAD_BUTTON_UP, KEYPAD_BUTTON_DOWN)) {
                state->menu_pending = true;
                state->up_repeat = 0;
                state->down_repeat = 0;
            }
        }
    }

    if (state->display_dirty) {
        float reading = densitometer_get_last_reading(state->densitometer);
        display_main_elements_t elements = {
            .title = state->display_title,
            .mode = state->display_mode,
            .density100 = ((!isnanf(reading)) ? lroundf(reading * 100) : 0),
            .zero_indicator = densitometer_has_zero(state->densitometer)
        };
        display_draw_main_elements(&elements);
        state->display_dirty = false;
    }
}
