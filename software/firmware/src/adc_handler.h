/*
 * Handles functions of the ADC peripheral, which includes measurements
 * of the internal voltage reference (VREF) and temperature sensor.
 */

#ifndef ADC_HANDLER_H
#define ADC_HANDLER_H

#include <stdint.h>
#include <cmsis_os.h>
#include "stm32l0xx_hal.h"

typedef struct {
    int16_t temp_c;
    uint16_t vdda_mv;
} adc_readings_t;

osStatus_t adc_handler_init();

/*
 * Read the internal ADC sources.
 * Note: This function will block on the completion of the ADC conversion.
 */
osStatus_t adc_read(adc_readings_t *readings);

void adc_completion_callback();

#endif /* ADC_HANDLER_H */
