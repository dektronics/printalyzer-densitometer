#include "state_measure.h"

#define LOG_TAG "state_measure"

#include <stdbool.h>
#include <elog.h>

#include "keypad.h"
#include "display.h"
#include "light.h"
#include "densitometer.h"

typedef struct {
    state_t base;
    bool display_dirty;
    bool take_measurement;
} state_measure_t;

static void state_reflection_measure_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_reflection_measure_process(state_t *state_base, state_controller_t *controller);
static state_measure_t state_reflection_measure_data = {
    .base = {
        .state_entry = state_reflection_measure_entry,
        .state_process = state_reflection_measure_process,
        .state_exit = NULL
    },
    .display_dirty = true,
    .take_measurement = true,
};

static void state_transmission_measure_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_transmission_measure_process(state_t *state_base, state_controller_t *controller);
static state_measure_t state_transmission_measure_data = {
    .base = {
        .state_entry = state_transmission_measure_entry,
        .state_process = state_transmission_measure_process,
        .state_exit = NULL
    },
    .display_dirty = true,
    .take_measurement = true,
};

static void sensor_read_callback(void *user_data);

state_t *state_reflection_measure()
{
    return (state_t *)&state_reflection_measure_data;
}

void state_reflection_measure_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_measure_t *state = (state_measure_t *)state_base;
    state->display_dirty = true;
    state->take_measurement = true;
}

void state_reflection_measure_process(state_t *state_base, state_controller_t *controller)
{
    state_measure_t *state = (state_measure_t *)state_base;

    display_main_elements_t elements = {
        .title = "Measuring...",
        .mode = DISPLAY_MODE_REFLECTION,
        .density100 = 0,
        .frame = 0
    };

    if (state->take_measurement) {
        display_draw_main_elements(&elements);

        densitometer_result_t result = densitometer_reflection_measure(sensor_read_callback, &elements);
        if (result == DENSITOMETER_CAL_ERROR) {
            display_static_list("Reflection",
                "Invalid\n"
                "calibration");
            HAL_Delay(2000);
            state_controller_set_next_state(controller, STATE_REFLECTION_DISPLAY);
        } else if (result == DENSITOMETER_SENSOR_ERROR) {
            display_static_list("Reflection",
                "Sensor\n"
                "read error");
            HAL_Delay(2000);
            state_controller_set_next_state(controller, STATE_REFLECTION_DISPLAY);
        } else {
            uint8_t key_state = keypad_get_state(0);

            state->take_measurement = false;
            state->display_dirty = true;

            /* Only return to the display state if the measure button was released */
            if ((key_state & KEYPAD_BUTTON_1) == 0) {
                state_controller_set_next_state(controller, STATE_REFLECTION_DISPLAY);
            }
        }
    } else {
        bool key_changed = false;
        uint8_t key_state = keypad_get_state(&key_changed);

        if (key_changed) {
            if ((key_state & KEYPAD_BUTTON_1) && (key_state & KEYPAD_BUTTON_2)) {
                log_i("Setting zero measurement to target");
                densitometer_reflection_set_zero();
                state->display_dirty = true;
            } else if ((key_state & KEYPAD_BUTTON_1) && (key_state & KEYPAD_BUTTON_3)) {
                log_i("Clear zero measurement");
                densitometer_reflection_clear_zero();
                state->display_dirty = true;
            } else if (key_state == 0) {
                state_controller_set_next_state(controller, STATE_REFLECTION_DISPLAY);
            }
        }

        if (state->display_dirty) {
            float reading = densitometer_reflection_get_last_reading();
            elements.density100 = (!isnanf(reading)) ? lroundf(reading * 100) : 0;
            elements.zero_indicator = densitometer_reflection_has_zero();
            display_draw_main_elements(&elements);
            state->display_dirty = false;
        }
    }
}

state_t *state_transmission_measure()
{
    return (state_t *)&state_transmission_measure_data;
}

void state_transmission_measure_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_measure_t *state = (state_measure_t *)state_base;
    state->display_dirty = true;
    state->take_measurement = true;
}

void state_transmission_measure_process(state_t *state_base, state_controller_t *controller)
{
    state_measure_t *state = (state_measure_t *)state_base;

    display_main_elements_t elements = {
        .title = "Measuring...",
        .mode = DISPLAY_MODE_TRANSMISSION,
        .density100 = 0,
        .frame = 0
    };

    if (state->take_measurement) {
        display_draw_main_elements(&elements);

        densitometer_result_t result = densitometer_transmission_measure(sensor_read_callback, &elements);
        if (result == DENSITOMETER_CAL_ERROR) {
            display_static_list("Transmission",
                "Invalid\n"
                "calibration");
            HAL_Delay(2000);
            state_controller_set_next_state(controller, STATE_TRANSMISSION_DISPLAY);
        } else if (result == DENSITOMETER_SENSOR_ERROR) {
            display_static_list("Transmission",
                "Sensor\n"
                "read error");
            HAL_Delay(2000);
            state_controller_set_next_state(controller, STATE_TRANSMISSION_DISPLAY);
        } else {
            uint8_t key_state = keypad_get_state(0);

            state->take_measurement = false;
            state->display_dirty = true;

            /* Only return to the display state if the measure button was released */
            if ((key_state & KEYPAD_BUTTON_1) == 0) {
                state_controller_set_next_state(controller, STATE_TRANSMISSION_DISPLAY);
            }
        }
    } else {
        bool key_changed = false;
        uint8_t key_state = keypad_get_state(&key_changed);

        if (key_changed) {
            if ((key_state & KEYPAD_BUTTON_1) && (key_state & KEYPAD_BUTTON_2)) {
                log_i("Setting zero measurement to target");
                densitometer_transmission_set_zero();
                state->display_dirty = true;
            } else if ((key_state & KEYPAD_BUTTON_1) && (key_state & KEYPAD_BUTTON_3)) {
                log_i("Clear zero measurement");
                densitometer_transmission_clear_zero();
                state->display_dirty = true;
            } else if (key_state == 0) {
                state_controller_set_next_state(controller, STATE_TRANSMISSION_DISPLAY);
            }
        }

        if (state->display_dirty) {
            float reading = densitometer_transmission_get_last_reading();
            elements.density100 = (!isnanf(reading)) ? lroundf(reading * 100) : 0;
            elements.zero_indicator = densitometer_transmission_has_zero();
            display_draw_main_elements(&elements);
            state->display_dirty = false;
        }
    }
}

void sensor_read_callback(void *user_data)
{
    display_main_elements_t *elements = (display_main_elements_t *)user_data;
    elements->frame++;
    if (elements->frame > 2) { elements->frame = 0; }
    display_draw_main_elements(elements);
}
