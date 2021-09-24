/*
 * Main task that initializes the system, creates all the other tasks,
 * and then runs the state controller.
 */

#ifndef TASK_MAIN_H
#define TASK_MAIN_H

#include <cmsis_os.h>

/**
 * Creates the main task which will run when the scheduler is started.
 */
osStatus_t task_main_init();

#endif /* TASK_MAIN_H */
