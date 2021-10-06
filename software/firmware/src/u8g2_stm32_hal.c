#include "u8g2_stm32_hal.h"

#define LOG_TAG "u8g2"

#include <elog.h>
#include <cmsis_os.h>

#include "stm32l0xx_hal.h"
#include "board_config.h"

static SPI_HandleTypeDef *u8g2_hspi;

void u8g2_stm32_hal_init(SPI_HandleTypeDef *hspi)
{
    u8g2_hspi = hspi;
}

uint8_t u8g2_stm32_spi_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    /* log_i("spi_byte_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p", msg, arg_int, arg_ptr); */
    switch (msg) {
    case U8X8_MSG_BYTE_SET_DC:
        /* Set DC to arg_int */
        HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    case U8X8_MSG_BYTE_INIT:
        /* Disable chip select */
        HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, GPIO_PIN_SET);
        break;
    case U8X8_MSG_BYTE_SEND: {
        /* Transmit bytes in arg_ptr, length is arg_int bytes */
        HAL_StatusTypeDef ret = HAL_SPI_Transmit(u8g2_hspi, (uint8_t *)arg_ptr, arg_int, HAL_MAX_DELAY);
        if (ret != HAL_OK) {
            log_e("HAL_SPI_Transmit error: %d", ret);
        }
        break;
    }
    case U8X8_MSG_BYTE_START_TRANSFER:
        /* Drop CS low to enable */
        HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, GPIO_PIN_RESET);
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        /* Bring CS high to disable */
        HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, GPIO_PIN_SET);
        break;
    default:
        return 0;
    }
    return 1;
}

uint8_t u8g2_stm32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    /* log_i("gpio_and_delay_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p", msg, arg_int, arg_ptr); */
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        /*
         * Initialize the GPIO and DELAY HAL functions,
         * pin init for DC, RESET, CS.
         * These are all initialized elsewhere.
         */
        osDelay(1);
        break;
    case U8X8_MSG_DELAY_MILLI:
        /* Delay for the number of milliseconds passed in through arg_int */
        osDelay(arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        /* Set the GPIO chip select pin to the value passed in through arg_int */
        HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    case U8X8_MSG_GPIO_RESET:
        /* Set the GPIO reset pin to the value passed in through arg_int */
        HAL_GPIO_WritePin(DISP_RES_GPIO_Port, DISP_RES_Pin, arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    default:
        return 0;
    }
    return 1;
}
