/**
 ******************************************************************************
 * @file         stm32l0xx_hal_msp.c
 * @brief        This file provides code for the MSP Initialization
 *               and de-Initialization codes.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include "stm32l0xx_hal.h"
#include "board_config.h"

extern DMA_HandleTypeDef hdma_adc;

extern void error_handler(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/**
 * Initializes the Global MSP
 */
void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    /* System interrupt init*/
    /* PendSV_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(PendSV_IRQn, 3, 0);
}

/**
 * ADC MSP Initialization
 *
 * @param hadc: ADC handle pointer
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1) {
        /* Peripheral clock enable */
        __HAL_RCC_ADC1_CLK_ENABLE();

        /* ADC1 DMA Init */
        /* ADC Init */
        hdma_adc.Instance = DMA1_Channel1;
        hdma_adc.Init.Request = DMA_REQUEST_0;
        hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc.Init.Mode = DMA_NORMAL;
        hdma_adc.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_adc) != HAL_OK) {
            error_handler();
        }

        __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);
    }
}

/**
 * ADC MSP De-Initialization
 *
 * @param hadc: ADC handle pointer
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1) {
        /* Peripheral clock disable */
        __HAL_RCC_ADC1_CLK_DISABLE();

        /* ADC1 DMA DeInit */
        HAL_DMA_DeInit(hadc->DMA_Handle);
    }
}

/**
 * I2C MSP Initialization
 *
 * @param hi2c: I2C handle pointer
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (hi2c->Instance == I2C1) {
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /*
         * I2C1 GPIO Configuration
         * PB6     ------> I2C1_SCL
         * PB7     ------> I2C1_SDA
         */
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __HAL_RCC_I2C1_CLK_ENABLE();
    }
}

/**
 * I2C MSP De-Initialization
 *
 * @param hi2c: I2C handle pointer
 */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
    if (hi2c->Instance == I2C1) {
        /* Peripheral clock disable */
        __HAL_RCC_I2C1_CLK_DISABLE();

        /*
         * I2C1 GPIO Configuration
         * PB6     ------> I2C1_SCL
         * PB7     ------> I2C1_SDA
         */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
    }
}

/**
 * SPI MSP Initialization
 *
 * @param hspi: SPI handle pointer
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (hspi->Instance == SPI1) {
        /* Peripheral clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /*
         * SPI1 GPIO Configuration
         * PA5     ------> SPI1_SCK
         * PA7     ------> SPI1_MOSI
         */
        GPIO_InitStruct.Pin = DISP_SCK_Pin | DISP_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/**
 * SPI MSP De-Initialization
 *
 * @param hspi: SPI handle pointer
 */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
    if (hspi->Instance == SPI1) {
        /* Peripheral clock disable */
        __HAL_RCC_SPI1_CLK_DISABLE();

        /*
         * SPI1 GPIO Configuration
         * PA5     ------> SPI1_SCK
         * PA7     ------> SPI1_MOSI
         */
        HAL_GPIO_DeInit(GPIOA, DISP_SCK_Pin | DISP_MOSI_Pin);
    }
}

/**
 * TIM_Base MSP Initialization
 *
 * @param htim_base: TIM_Base handle pointer
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
    if (htim_base->Instance == TIM2) {
        /* Peripheral clock enable */
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (htim->Instance == TIM2) {
        /* Peripheral clock enable */
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /*
         * TIM2 GPIO Configuration
         * PA15    ------> TIM2_CH1
         * PB3     ------> TIM2_CH2
         */
        GPIO_InitStruct.Pin = TLED_EN_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF5_TIM2;
        HAL_GPIO_Init(TLED_EN_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = RLED_EN_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
        HAL_GPIO_Init(RLED_EN_GPIO_Port, &GPIO_InitStruct);
    }
}

/**
 * TIM_Base MSP De-Initialization
 *
 * @param htim_base: TIM_Base handle pointer
 */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
    if (htim_base->Instance == TIM2) {
        /* Peripheral clock disable */
        __HAL_RCC_TIM2_CLK_DISABLE();
    }
}

/**
 * @brief UART MSP Initialization
 *
 * @param huart: UART handle pointer
 */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (huart->Instance == USART1) {
        /* Peripheral clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /*
         * USART1 GPIO Configuration
         * PA9     ------> USART1_TX
         * PA10    ------> USART1_RX
         */
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/**
 * @brief UART MSP De-Initialization
 *
 * @param huart: UART handle pointer
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
    if (huart->Instance == USART1) {
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /*
         * USART1 GPIO Configuration
         * PA9     ------> USART1_TX
         * PA10    ------> USART1_RX
         */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
    }
}
