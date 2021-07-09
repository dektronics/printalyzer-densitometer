#ifndef LIGHT_H
#define LIGHT_H

#include "stm32l0xx_hal.h"

#define LIGHT_REFLECTION_IDLE       32
#define LIGHT_REFLECTION_MEASURE   128
#define LIGHT_TRANSMISSION_IDLE      8
#define LIGHT_TRANSMISSION_MEASURE 128

void light_init(TIM_HandleTypeDef *htim, uint32_t r_channel, uint32_t t_channel);

void light_set_reflection(uint8_t val);
void light_set_transmission(uint8_t val);

#endif /* LIGHT_H */
