#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <cmsis_os.h>
#include "stm32l0xx_hal.h"

osStatus_t hal_to_os_status(HAL_StatusTypeDef hal_status);
HAL_StatusTypeDef os_to_hal_status(osStatus_t os_status);

#endif /* UTIL_H */
