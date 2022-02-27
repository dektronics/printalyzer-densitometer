#include "adc_handler.h"

#include "stm32l0xx_hal.h"
#include "stm32l0xx_ll_adc.h"
#include <cmsis_os.h>

#define LOG_TAG "adc"
#include <elog.h>

#include "util.h"

extern ADC_HandleTypeDef hadc;

/* Size of buffer to store ADC DMA results */
#define ADC_BUFFER_SIZE ((uint32_t)2)

static volatile bool adc_initialized = false;
static __IO uint16_t adc_converted_values[ADC_BUFFER_SIZE];

static osSemaphoreId_t adc_semaphore = NULL;
static const osSemaphoreAttr_t adc_semaphore_attrs = {
    .name = "adc_semaphore"
};

osStatus_t adc_handler_init()
{
    osStatus_t ret = osOK;
    HAL_StatusTypeDef hret = HAL_OK;

    do {
        /* Create the semaphore used to synchronize blocking reads */
        adc_semaphore = osSemaphoreNew(1, 0, &adc_semaphore_attrs);
        if (!adc_semaphore) {
            log_e("adc_semaphore create error");
            ret = osErrorNoMemory;
            break;
        }

        /* Run the ADC calibration in single-ended mode */
        hret = HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
        if (hret != HAL_OK) {
            log_e("Error starting ADC calibration: %d", ret);
            ret = hal_to_os_status(hret);
            break;
        }
    } while (0);

    log_i("ADC handler initialized");

    return ret;
}

osStatus_t adc_read(adc_readings_t *readings)
{
    osStatus_t ret = osOK;
    HAL_StatusTypeDef hret = HAL_OK;

    if (!readings) {
        return osErrorParameter;
    }

    do {
        /* Start ADC conversion on regular group with transfer by DMA */
        hret = HAL_ADC_Start_DMA(&hadc, (uint32_t *)adc_converted_values, ADC_BUFFER_SIZE);
        if (hret != HAL_OK) {
            log_e("Error starting ADC DMA: %d", ret);
            ret = hal_to_os_status(hret);
            break;
        }

        /* Block on completion of the conversion */
        ret = osSemaphoreAcquire(adc_semaphore, 1000);
        if (ret != osOK) {
            log_e("Unable to acquire adc_semaphore");
            break;
        }

        /* Stop ADC conversion */
        hret = HAL_ADC_Stop_DMA(&hadc);
        if (hret != HAL_OK) {
            log_e("Error stopping ADC DMA: %d", ret);
            ret = hal_to_os_status(hret);
            break;
        }

        /* Copy the converted values into 32-bit integers */
        uint32_t vref_meas = adc_converted_values[0];
        uint32_t temp_meas = adc_converted_values[1];

        /* Calculate the VDDA value in mV */
        uint16_t vdda = __LL_ADC_CALC_VREFANALOG_VOLTAGE(vref_meas, LL_ADC_RESOLUTION_12B);

        /* Calculate the temperature reading using VDDA */
        int32_t temperature = __LL_ADC_CALC_TEMPERATURE(vdda, temp_meas, LL_ADC_RESOLUTION_12B);

        /* Copy the results to the struct */
        readings->temp_c = temperature;
        readings->vdda_mv = vdda;
    } while (0);

    return ret;
}

void adc_completion_callback()
{
    osSemaphoreRelease(adc_semaphore);
}
