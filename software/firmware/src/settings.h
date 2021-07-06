#ifndef SETTINGS_H
#define SETTINGS_H

#include "stm32l0xx_hal.h"

#include <stdbool.h>

#include "tsl2591.h"

HAL_StatusTypeDef settings_init();

void settings_set_calibration_gain(tsl2591_gain_t gain, float ch0_gain, float ch1_gain);
void settings_get_calibration_gain(tsl2591_gain_t gain, float *ch0_gain, float *ch1_gain);

#endif /* SETTINGS_H */
