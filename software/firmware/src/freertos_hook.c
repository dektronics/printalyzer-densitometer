#define LOG_TAG "freertos"
#include <elog.h>

#include "stm32l0xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

extern IWDG_HandleTypeDef hiwdg;

void vApplicationMallocFailedHook(void)
{
    log_e("Malloc failed!");
    taskDISABLE_INTERRUPTS();
    __ASM volatile("BKPT #01");
    while (1) { }
}

void vApplicationStackOverflowHook(xTaskHandle pxTask, char *pcTaskName)
{
    UNUSED(pxTask);

    log_e("Stack overflow! task=\"%s\"", pcTaskName);
    taskDISABLE_INTERRUPTS();
    __ASM volatile("BKPT #01");
    while (1) { }
}

void vApplicationIdleHook(void)
{
    /*
     * Kicking the watchdog here mostly protects against a task hogging the
     * whole system and preventing the scheduler from doing its thing.
     */
    HAL_IWDG_Refresh(&hiwdg);
}
