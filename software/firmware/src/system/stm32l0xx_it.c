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

//extern PCD_HandleTypeDef hpcd_USB_FS;

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

/**
 * Handles the system service call via SWI instruction.
 */
void SVC_Handler(void)
{
}

/**
 * Handles the pendable request for system service.
 */
void PendSV_Handler(void)
{
}

/**
 * Handles the system tick timer.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

/**
 * Handles the EXTI line 2 and line 3 interrupts.
 */
void EXTI2_3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

/**
 * Handles the EXTI line 4 to 15 interrupts.
 */
void EXTI4_15_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

/**
 * Handles the USB event/wake-up interrupt through EXTI line 18.
 */
void USB_IRQHandler(void)
{
    //HAL_PCD_IRQHandler(&hpcd_USB_FS);
}
