/*
 * Main task that initializes the system, creates all the other tasks,
 * and then runs the state controller.
 */

#ifndef TASK_MAIN_H
#define TASK_MAIN_H

#include <stdbool.h>
#include <cmsis_os.h>
#include "state_controller.h"

/**
 * Creates the main task which will run when the scheduler is started.
 */
osStatus_t task_main_init();

/**
 * Gets whether the main task has completed initialization.
 */
bool task_main_is_running();

/**
 * Send a notification to the main task to force a state transition
 *
 * @param next_state State to switch to
 * @return osOK on success
 */
osStatus_t task_main_force_state(state_identifier_t next_state);

#endif /* TASK_MAIN_H */
