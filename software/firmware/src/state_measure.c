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
static state_measure_t state_reflection_measure_data = {
    .base = {
        .state_entry = state_reflection_measure_entry,
        .state_process = state_reflection_measure_process,
        .state_exit = NULL
    },
    .display_dirty = true
};

static void state_transmission_measure_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_transmission_measure_process(state_t *state_base, state_controller_t *controller);
static state_measure_t state_transmission_measure_data = {
    .base = {
        .state_entry = state_transmission_measure_entry,
        .state_process = state_transmission_measure_process,
        .state_exit = NULL
    },
    .display_dirty = true
};

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
    state_measure_t *state = (state_measure_t *)state_base;

    if (state->display_dirty) {
        display_clear();
        state->display_dirty = false;
    }
    densitometer_reflection_measure();
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
    state_measure_t *state = (state_measure_t *)state_base;

    if (state->display_dirty) {
        display_clear();
        state->display_dirty = false;
    }
    densitometer_transmission_measure();
    state_controller_set_next_state(controller, STATE_TRANSMISSION_DISPLAY);
}
