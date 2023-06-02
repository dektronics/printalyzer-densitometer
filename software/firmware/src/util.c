#include "util.h"

#include <string.h>
#include <math.h>
#include "stm32l0xx_hal.h"

#ifdef HAL_IWDG_MODULE_ENABLED
extern IWDG_HandleTypeDef hiwdg;
#endif

void watchdog_refresh()
{
#ifdef HAL_IWDG_MODULE_ENABLED
    HAL_IWDG_Refresh(&hiwdg);
#endif
}

void watchdog_slow()
{
#ifdef HAL_IWDG_MODULE_ENABLED
    hiwdg.Init.Prescaler = IWDG_PRESCALER_16;
    HAL_IWDG_Init(&hiwdg);
#endif
}

void watchdog_normal()
{
#ifdef HAL_IWDG_MODULE_ENABLED
    hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
    HAL_IWDG_Init(&hiwdg);
#endif
}

osStatus_t hal_to_os_status(HAL_StatusTypeDef hal_status)
{
    switch (hal_status) {
    case HAL_OK:
        return osOK;
    case HAL_ERROR:
        return osError;
    case HAL_BUSY:
        return osErrorResource;
    case HAL_TIMEOUT:
        return osErrorTimeout;
    default:
        return osError;
    }
}

HAL_StatusTypeDef os_to_hal_status(osStatus_t os_status)
{
    switch (os_status) {
    case osOK:
        return HAL_OK;
    case osError:
        return HAL_ERROR;
    case osErrorTimeout:
        return HAL_TIMEOUT;
    case osErrorResource:
        return HAL_BUSY;
    case osErrorParameter:
    case osErrorNoMemory:
    case osErrorISR:
    default:
        return HAL_ERROR;
    }
}

void copy_from_u32(uint8_t *buf, uint32_t val)
{
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

uint32_t copy_to_u32(const uint8_t *buf)
{
    return (uint32_t)buf[0] << 24
        | (uint32_t)buf[1] << 16
        | (uint32_t)buf[2] << 8
        | (uint32_t)buf[3];
}

void copy_from_f32(uint8_t *buf, float val)
{
    uint32_t int_val;
    memcpy(&int_val, &val, sizeof(float));
    copy_from_u32(buf, int_val);
}

float copy_to_f32(const uint8_t *buf)
{
    float val;
    uint32_t int_val = copy_to_u32(buf);
    memcpy(&val, &int_val, sizeof(float));
    return val;
}

bool is_valid_number(float num)
{
    return isnormal(num) || fpclassify(num) == FP_ZERO;
}

void replace_first_char(char *str, char orig, char repl)
{
    char *p = strchr(str, orig);
    if (p) {
        *p = repl;
    }
}

void replace_all_char(char *str, char orig, char repl)
{
    char *p = str;
    while((p = strchr(p, orig)) != NULL) {
        *p++ = repl;
    }
}
