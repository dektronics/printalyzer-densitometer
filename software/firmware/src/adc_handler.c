#include "adc_handler.h"

#include "stm32l0xx_hal.h"
#include "stm32l0xx_ll_adc.h"
#include <cmsis_os.h>
#include <math.h>

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

        /* Capture various constants as floating point values */
        const float vrefint_cal = (float)((uint32_t)(*VREFINT_CAL_ADDR));
        const float vrefint_cal_vref = (float)VREFINT_CAL_VREF;
        const float temp_cal_vref = (float)TEMPSENSOR_CAL_VREFANALOG;
        const float temp_cal1 = (float)((int32_t)(*TEMPSENSOR_CAL1_ADDR));
        const float temp_cal2 = (float)((int32_t)(*TEMPSENSOR_CAL2_ADDR));
        const float temp_cal1_temp = (float)TEMPSENSOR_CAL1_TEMP;
        const float temp_cal2_temp = (float)TEMPSENSOR_CAL2_TEMP;

        /* Convert the oversampled values into floating point */
        float vref_meas = (float)adc_converted_values[0] / 16.0F;
        float temp_meas = (float)adc_converted_values[1] / 16.0F;

        /* Calculate the VDDA value in mV */
        float vdda = (vrefint_cal_vref * vrefint_cal) / vref_meas;

        /* Calculate the temperature reading using VDDA */
        float temperature;
        temperature = (temp_meas * vdda / temp_cal_vref) - temp_cal1;
        temperature *= temp_cal2_temp - temp_cal1_temp;
        temperature /= temp_cal2 - temp_cal1;
        temperature += temp_cal1_temp;

        /* Round and copy the results to the struct */
        readings->temp_c = lroundf(temperature);
        readings->vdda_mv = lroundf(vdda);

    } while (0);

    return ret;
}

void adc_completion_callback()
{
    osSemaphoreRelease(adc_semaphore);
}
