/**
  ******************************************************************************
  * @file    stm32l0xx_it.h
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef __STM32L0xx_IT_H
#define __STM32L0xx_IT_H

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler(void);
void HardFault_Handler(void);
void RTC_IRQHandler(void);
void EXTI0_1_IRQHandler(void);
void EXTI4_15_IRQHandler(void);
void DMA1_Channel1_IRQHandler(void);
void TIM6_DAC_IRQHandler(void);
void USB_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32L0xx_IT_H */
