#include "state_home.h"

#define LOG_TAG "state_home"
#include <elog.h>

#include <cmsis_os.h>
#include <stdbool.h>

#include "state_controller.h"
#include "task_sensor.h"
#include "display.h"

typedef struct {
    state_t base;
    bool first_run;
} state_home_t;


static void state_home_process(state_t *state_base, state_controller_t *controller);
static state_home_t state_home_data = {
    .base = {
        .state_entry = NULL,
        .state_process = state_home_process,
        .state_exit = NULL
    },
    .first_run = true
};

state_t *state_home()
{
    return (state_t *)&state_home_data;
}

void state_home_process(state_t *state_base, state_controller_t *controller)
{
    state_home_t *state = (state_home_t *)state_base;

    if (state->first_run) {
        if (!sensor_is_initialized()) {
            display_message(
                "Sensor", NULL,
                "initialization\n"
                "failed", " OK ");
        }
        state->first_run = false;
    } else {
        /*
         * After startup, the home state does not really do anything.
         * It just decides which of the main device states to transition to.
         */
        state_identifier_t home_state = state_controller_get_home_state(controller);
        state_controller_set_next_state(controller, home_state);
    }
}
