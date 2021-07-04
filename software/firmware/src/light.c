#include "light.h"

#include "stm32l0xx_hal.h"

static TIM_HandleTypeDef *light_htim;
static uint32_t light_r_channel;
static uint32_t light_t_channel;

void light_init(TIM_HandleTypeDef *htim, uint32_t r_channel, uint32_t t_channel)
{
    light_htim = htim;
    light_r_channel = r_channel;
    light_t_channel = t_channel;

    /* Set the lights to off and enable the PWM timer */
    __HAL_TIM_SET_COMPARE(light_htim, light_r_channel, 0);
    __HAL_TIM_SET_COMPARE(light_htim, light_t_channel, 0);
    HAL_TIM_PWM_Start(light_htim, light_r_channel);
    HAL_TIM_PWM_Start(light_htim, light_t_channel);
}

void light_set_reflection(uint8_t val)
{
    __HAL_TIM_SET_COMPARE(light_htim, light_r_channel, val);
}

void light_set_transmission(uint8_t val)
{
    __HAL_TIM_SET_COMPARE(light_htim, light_t_channel, val);
}
