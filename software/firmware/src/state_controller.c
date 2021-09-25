#include "state_controller.h"

#define LOG_TAG "state_controller"
#include <elog.h>

#include <stdio.h>
#include <cmsis_os.h>

#include "state_display.h"
#include "state_measure.h"
#include "state_main_menu.h"

struct __state_controller_t {
    state_identifier_t current_state;
    state_identifier_t next_state;
};

static state_controller_t state_controller = {0};
static state_t *state_map[STATE_MAX] = {0};

static state_t *state_home();
static void state_home_process(state_t *state_base, state_controller_t *controller);
static state_t state_home_data = {
    .state_entry = NULL,
    .state_process = state_home_process,
    .state_exit = NULL
};

void state_controller_init()
{
    state_controller.current_state = STATE_MAX;
    state_controller.next_state = STATE_HOME;

    state_map[STATE_HOME] = state_home();
    state_map[STATE_REFLECTION_DISPLAY] = state_reflection_display();
    state_map[STATE_REFLECTION_MEASURE] = state_reflection_measure();
    state_map[STATE_TRANSMISSION_DISPLAY] = state_transmission_display();
    state_map[STATE_TRANSMISSION_MEASURE] = state_transmission_measure();
    state_map[STATE_MAIN_MENU] = state_main_menu();
}

void state_controller_loop()
{
    state_t *state = NULL;
    for (;;) {
        /* Check if we need to do a state transition */
        if (state_controller.next_state != state_controller.current_state) {
            /* Transition to the new state */
            log_i("State transition: %d -> %d",
                state_controller.current_state, state_controller.next_state);
            state_identifier_t prev_state = state_controller.current_state;
            state_controller.current_state = state_controller.next_state;

            if (state_controller.current_state < STATE_MAX && state_map[state_controller.current_state]) {
                state = state_map[state_controller.current_state];
            } else {
                state = NULL;
            }

            /* Call the state entry function */
            if (state && state->state_entry) {
                state->state_entry(state, &state_controller, prev_state);
            }
        }

        /* Call the process function for the state */
        if (state && state->state_process) {
            state->state_process(state, &state_controller);
        }

        /* Check if we will do a state transition on the next loop */
        if (state_controller.next_state != state_controller.current_state) {
            if (state && state->state_exit) {
                state->state_exit(state, &state_controller, state_controller.next_state);
            }
        }
    }
}

void state_controller_set_next_state(state_controller_t *controller, state_identifier_t next_state)
{
    if (!controller) { return; }
    controller->next_state = next_state;
}

state_t *state_home()
{
    return (state_t *)&state_home_data;
}

void state_home_process(state_t *state_base, state_controller_t *controller)
{
    /*
     * The home state does not really do anything. It just decides which
     * of the main device states to start in.
     */
    state_controller_set_next_state(controller, STATE_REFLECTION_DISPLAY);
}
