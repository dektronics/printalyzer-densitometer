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

extern DMA_HandleTypeDef hdma_adc;
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
    //TODO Revisit and clean up this code
    __asm(  ".syntax unified\n"
        "MOVS   R0, #4  \n"
        "MOV    R1, LR  \n"
        "TST    R0, R1  \n"
        "BEQ    _MSP    \n"
        "MRS    R0, PSP \n"
        "B      HardFault_HandlerC      \n"
        "_MSP:  \n"
        "MRS    R0, MSP \n"
        "B      HardFault_HandlerC      \n"
        ".syntax divided\n");
}

__attribute__((used)) void HardFault_HandlerC(unsigned long *hardfault_args)
{
    //TODO Revisit and clean up this code
    volatile unsigned long stacked_r0;
    volatile unsigned long stacked_r1;
    volatile unsigned long stacked_r2;
    volatile unsigned long stacked_r3;
    volatile unsigned long stacked_r12;
    volatile unsigned long stacked_lr;
    volatile unsigned long stacked_pc;
    volatile unsigned long stacked_psr;
    volatile unsigned long _CFSR;
    volatile unsigned long _HFSR;
    volatile unsigned long _DFSR;
    volatile unsigned long _AFSR;
    volatile unsigned long _BFAR;
    volatile unsigned long _MMAR;

    stacked_r0 = ((unsigned long)hardfault_args[0]);
    stacked_r1 = ((unsigned long)hardfault_args[1]);
    stacked_r2 = ((unsigned long)hardfault_args[2]);
    stacked_r3 = ((unsigned long)hardfault_args[3]);
    stacked_r12 = ((unsigned long)hardfault_args[4]);
    stacked_lr = ((unsigned long)hardfault_args[5]);
    stacked_pc = ((unsigned long)hardfault_args[6]);
    stacked_psr = ((unsigned long)hardfault_args[7]);

    // Configurable Fault Status Register
    // Consists of MMSR, BFSR and UFSR
    _CFSR = (*((volatile unsigned long *)(0xE000ED28)));

    // Hard Fault Status Register
    _HFSR = (*((volatile unsigned long *)(0xE000ED2C)));

    // Debug Fault Status Register
    _DFSR = (*((volatile unsigned long *)(0xE000ED30)));

    // Auxiliary Fault Status Register
    _AFSR = (*((volatile unsigned long *)(0xE000ED3C)));

    // Read the Fault Address Registers. These may not contain valid values.

    // Check BFARVALID/MMARVALID to see if they are valid values

    // MemManage Fault Address Register
    _MMAR = (*((volatile unsigned long *)(0xE000ED34)));

    // Bus Fault Address Register
    _BFAR = (*((volatile unsigned long *)(0xE000ED38)));

    // Silence compiler warnings
    UNUSED(stacked_r0);
    UNUSED(stacked_r1);
    UNUSED(stacked_r2);
    UNUSED(stacked_r3);
    UNUSED(stacked_r12);
    UNUSED(stacked_lr);
    UNUSED(stacked_pc);
    UNUSED(stacked_psr);
    UNUSED(_CFSR);
    UNUSED(_HFSR);
    UNUSED(_DFSR);
    UNUSED(_AFSR);
    UNUSED(_BFAR);
    UNUSED(_MMAR);

    __asm("BKPT #0\n") ; // Break into the debugger
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
 * Handles the DMA1 channel 1 interrupt.
 */
void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc);
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
