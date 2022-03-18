#ifndef STATE_CONTROLLER_H
#define STATE_CONTROLLER_H

#define STATE_KEYPAD_WAIT 200

typedef enum {
    STATE_HOME = 0,
    STATE_REFLECTION_DISPLAY,
    STATE_REFLECTION_MEASURE,
    STATE_TRANSMISSION_DISPLAY,
    STATE_TRANSMISSION_MEASURE,
    STATE_MAIN_MENU,
    STATE_CALIBRATION_SENSOR,
    STATE_CALIBRATION_REFLECTION,
    STATE_CALIBRATION_REFLECTION_CAL_LO,
    STATE_CALIBRATION_REFLECTION_CAL_HI,
    STATE_CALIBRATION_TRANSMISSION,
    STATE_CALIBRATION_TRANSMISSION_ZERO,
    STATE_CALIBRATION_TRANSMISSION_CAL_HI,
    STATE_REMOTE,
    STATE_SUSPEND,
    STATE_MAX
} state_identifier_t;

typedef struct __state_controller_t state_controller_t;
typedef struct __state_t state_t;

typedef void (*state_entry_func_t)(state_t *state, state_controller_t *controller, state_identifier_t prev_state);
typedef void (*state_process_func_t)(state_t *state, state_controller_t *controller);
typedef void (*state_exit_func_t)(state_t *state, state_controller_t *controller, state_identifier_t next_state);

struct __state_t {
    /**
     * Function called on entry into the state.
     */
    state_entry_func_t state_entry;

    /**
     * Function called on each processing loop within the state.
     */
    state_process_func_t state_process;

    /**
     * Function called on exit from the state.
     */
    state_exit_func_t state_exit;
};

void state_controller_init();
void state_controller_loop();

state_identifier_t state_controller_get_current_state(const state_controller_t *controller);

void state_controller_set_next_state(state_controller_t *controller, state_identifier_t next_state);

void state_controller_set_home_state(state_controller_t *controller, state_identifier_t home_state);

#endif /* STATE_CONTROLLER_H */
