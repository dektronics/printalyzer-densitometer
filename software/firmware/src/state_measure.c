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
} state_measure_t;

static void state_reflection_measure_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_reflection_measure_process(state_t *state_base, state_controller_t *controller);
static state_t state_reflection_measure_data = {
    .state_entry = state_reflection_measure_entry,
    .state_process = state_reflection_measure_process,
    .state_exit = NULL
};

static void state_transmission_measure_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_transmission_measure_process(state_t *state_base, state_controller_t *controller);
static state_t state_transmission_measure_data = {
    .state_entry = state_transmission_measure_entry,
    .state_process = state_transmission_measure_process,
    .state_exit = NULL
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
}

void state_reflection_measure_process(state_t *state_base, state_controller_t *controller)
{
    display_main_elements_t elements = {
        .title = "Measuring...",
        .mode = DISPLAY_MODE_REFLECTION,
        .density100 = 0,
        .frame = 0
    };
    display_draw_main_elements(&elements);

    densitometer_result_t result = densitometer_reflection_measure(sensor_read_callback, &elements);
    if (result == DENSITOMETER_CAL_ERROR) {
        display_static_list("Reflection",
            "Invalid\n"
            "calibration");
        HAL_Delay(2000);
    } else if (result == DENSITOMETER_SENSOR_ERROR) {
        display_static_list("Reflection",
            "Sensor\n"
            "read error");
        HAL_Delay(2000);
    }
    state_controller_set_next_state(controller, STATE_REFLECTION_DISPLAY);
}

state_t *state_transmission_measure()
{
    return (state_t *)&state_transmission_measure_data;
}

void state_transmission_measure_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_measure_t *state = (state_measure_t *)state_base;
    state->display_dirty = true;
}

void state_transmission_measure_process(state_t *state_base, state_controller_t *controller)
{
    display_main_elements_t elements = {
        .title = "Measuring...",
        .mode = DISPLAY_MODE_TRANSMISSION,
        .density100 = 0,
        .frame = 0
    };
    display_draw_main_elements(&elements);

    densitometer_result_t result = densitometer_transmission_measure(sensor_read_callback, &elements);
    if (result == DENSITOMETER_CAL_ERROR) {
        display_static_list("Transmission",
            "Invalid\n"
            "calibration");
        HAL_Delay(2000);
    } else if (result == DENSITOMETER_SENSOR_ERROR) {
        display_static_list("Transmission",
            "Sensor\n"
            "read error");
        HAL_Delay(2000);
    }
    state_controller_set_next_state(controller, STATE_TRANSMISSION_DISPLAY);
}

void sensor_read_callback(void *user_data)
{
    display_main_elements_t *elements = (display_main_elements_t *)user_data;
    elements->frame++;
    if (elements->frame > 2) { elements->frame = 0; }
    display_draw_main_elements(elements);
}
