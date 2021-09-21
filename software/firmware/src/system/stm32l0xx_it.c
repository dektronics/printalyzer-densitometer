/**
 ******************************************************************************
 * @file    stm32l0xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include "stm32l0xx_hal.h"
#include "stm32l0xx_it.h"
#include <tusb.h>

extern PCD_HandleTypeDef hpcd_USB_FS;
extern TIM_HandleTypeDef htim6;

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/

/**
 * Handles the non maskable interrupt.
 */
void NMI_Handler(void)
{
    __ASM volatile("BKPT #01");
    while (1) { }
}

/**
 * Handles the hard fault interrupt.
 */
void HardFault_Handler(void)
{
    __ASM volatile("BKPT #01");
    while (1) { }
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

/**
 * Handles the EXTI line 0 and line 1 interrupts.
 */
void EXTI0_1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

/**
 * Handles the EXTI line 4 to 15 interrupts.
 */
void EXTI4_15_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

/**
 * Handles the TIM6 global interrupt and DAC1/DAC2 underrun error interrupts.
 */
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim6);
}

/**
 * Handles the USB event/wake-up interrupt through EXTI line 18.
 */
void USB_IRQHandler(void)
{
    tud_int_handler(0);
}
