#define LOG_TAG "freertos"
#include <elog.h>

#include "stm32l0xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

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
