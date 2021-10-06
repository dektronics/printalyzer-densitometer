#include "util.h"

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
