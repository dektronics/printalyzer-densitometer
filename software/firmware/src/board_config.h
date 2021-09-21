#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "stm32l0xx_hal.h"

/* Pin mapping for input buttons */
#define BTN1_Pin            GPIO_PIN_1
#define BTN1_GPIO_Port      GPIOA
#define BTN1_EXTI_IRQn      EXTI0_1_IRQn
#define BTN2_Pin            GPIO_PIN_0
#define BTN2_GPIO_Port      GPIOA
#define BTN2_EXTI_IRQn      EXTI0_1_IRQn
#define BTN3_Pin            GPIO_PIN_15
#define BTN3_GPIO_Port      GPIOC
#define BTN3_EXTI_IRQn      EXTI4_15_IRQn
#define BTN4_Pin            GPIO_PIN_14
#define BTN4_GPIO_Port      GPIOC
#define BTN4_EXTI_IRQn      EXTI4_15_IRQn
#define BTN5_Pin            GPIO_PIN_8
#define BTN5_GPIO_Port      GPIOA
#define BTN5_EXTI_IRQn      EXTI4_15_IRQn

/* Pin mapping for LED drivers */
#define RLED_EN_Pin         GPIO_PIN_2
#define RLED_EN_GPIO_Port   GPIOA
#define TLED_EN_Pin         GPIO_PIN_3
#define TLED_EN_GPIO_Port   GPIOA

/* Pin mapping for display */
#define DISP_CS_Pin         GPIO_PIN_4
#define DISP_CS_GPIO_Port   GPIOA
#define DISP_SCK_Pin        GPIO_PIN_5
#define DISP_SCK_GPIO_Port  GPIOA
#define DISP_DC_Pin         GPIO_PIN_6
#define DISP_DC_GPIO_Port   GPIOA
#define DISP_MOSI_Pin       GPIO_PIN_7
#define DISP_MOSI_GPIO_Port GPIOA
#define DISP_RES_Pin        GPIO_PIN_0
#define DISP_RES_GPIO_Port  GPIOB

/* Pin mapping for sensor interrupt */
#define SENSOR_INT_Pin       GPIO_PIN_5
#define SENSOR_INT_GPIO_Port GPIOB
#define SENSOR_INT_EXTI_IRQn EXTI4_15_IRQn

#endif /* BOARD_CONFIG_H */
