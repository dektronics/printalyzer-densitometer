#include "state_remote.h"

#define LOG_TAG "state_remote"
#include <elog.h>

#include <cmsis_os.h>

#include "display.h"
#include "keypad.h"
#include "task_sensor.h"
#include "cdc_handler.h"

typedef struct {
    state_t base;
} state_remote_t;

static void state_remote_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_remote_process(state_t *state_base, state_controller_t *controller);
static void state_remote_exit(state_t *state, state_controller_t *controller, state_identifier_t next_state);
static state_remote_t state_remote_data = {
    .base = {
        .state_entry = state_remote_entry,
        .state_process = state_remote_process,
        .state_exit = state_remote_exit
    }
};

state_t *state_remote()
{
    return (state_t *)&state_remote_data;
}

void state_remote_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    log_i("Entering remote control state");
    sensor_set_light_mode(SENSOR_LIGHT_OFF, false, 0);
    display_static_message("Remote\nControl");
    cdc_send_remote_state(true);
}

void state_remote_process(state_t *state_base, state_controller_t *controller)
{
    /* Wait on the keypad queue to avoid letting irrelevant events pile up */
    keypad_event_t keypad_event;
    keypad_wait_for_event(&keypad_event, 50);
    UNUSED(keypad_event);
}

void state_remote_exit(state_t *state, state_controller_t *controller, state_identifier_t next_state)
{
    log_i("Leaving remote control state");
    sensor_set_light_mode(SENSOR_LIGHT_OFF, false, 0);
    sensor_stop();
    display_clear();
    cdc_send_remote_state(false);
}
