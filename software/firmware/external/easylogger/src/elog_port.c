/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */

#include "elog_port.h"

#include <elog.h>
#include <printf.h>
#include <unistd.h>
#include <cmsis_os.h>
#include "stm32l0xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "cdc_handler.h"

static osMutexId_t elog_mutex = NULL;
static const osMutexAttr_t elog_mutex_attributes = {
    .name = "elog_mutex",
    .attr_bits = osMutexRecursive
};

static elog_port_output_callback_t output_callback = NULL;

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void)
{
    ElogErrCode result = ELOG_NO_ERR;
    elog_mutex = osMutexNew(&elog_mutex_attributes);
    return result;
}

/**
 * EasyLogger port deinitialize
 *
 */
void elog_port_deinit(void)
{
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size)
{
    if (output_callback) {
        output_callback(log, size);
    } else {
        write(1, log, size);
    }
}

/**
 * output lock
 */
void elog_port_output_lock(void)
{
    if (osKernelGetState() == osKernelRunning) {
        osMutexAcquire(elog_mutex, portMAX_DELAY);
    } else {
        __disable_irq();
    }
}

/**
 * output unlock
 */
void elog_port_output_unlock(void)
{
    if (osKernelGetState() == osKernelRunning) {
        osMutexRelease(elog_mutex);
    } else {
        __enable_irq();
    }
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void)
{
    static char tick_count[16] = {0};
    uint32_t tick_val;
    if (osKernelGetState() == osKernelRunning) {
        tick_val = osKernelGetTickCount();
    } else {
        tick_val = HAL_GetTick();
    }
    snprintf(tick_count, 16, "%10lu", tick_val);
    return tick_count;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void)
{
    /* No PID */
    return "";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void)
{
    static char t_info[16] = {0};
    if (osKernelGetState() == osKernelRunning) {
        snprintf(t_info, 16, "%s", osThreadGetName(osThreadGetId()));
    } else {
        t_info[0] = '\0';
    }
    return t_info;
}

/**
 * Assign a callback to redirect log output
 */
void elog_port_redirect(elog_port_output_callback_t callback)
{
    elog_port_output_lock();
    output_callback = callback;
    elog_port_output_unlock();
}
