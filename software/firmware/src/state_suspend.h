#ifndef STATE_SUSPEND_H
#define STATE_SUSPEND_H

#include "state_controller.h"

state_t *state_suspend();

void state_suspend_usb_wakeup_handler();
void state_suspend_rtc_wakeup_handler();

#endif /* STATE_SUSPEND_H */
