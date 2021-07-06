#include "settings.h"

#define LOG_TAG "settings"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <elog.h>

static HAL_StatusTypeDef settings_read_buffer(uint32_t address, uint8_t *data, size_t data_len);
static HAL_StatusTypeDef settings_write_buffer(uint32_t address, const uint8_t *data, size_t data_len);
static HAL_StatusTypeDef settings_write_float(uint32_t address, float val);

#define CONFIG_HEADER (DATA_EEPROM_BASE + 0U)

#define CONFIG_CAL_BASE (DATA_EEPROM_BASE + 128U)
#define CONFIG_CAL_GAIN_MEDIUM_CH0  (CONFIG_CAL_BASE + 0U)
#define CONFIG_CAL_GAIN_MEDIUM_CH1  (CONFIG_CAL_BASE + 4U)
#define CONFIG_CAL_GAIN_HIGH_CH0    (CONFIG_CAL_BASE + 8U)
#define CONFIG_CAL_GAIN_HIGH_CH1    (CONFIG_CAL_BASE + 12U)
#define CONFIG_CAL_GAIN_MAXIMUM_CH0 (CONFIG_CAL_BASE + 16U)
#define CONFIG_CAL_GAIN_MAXIMUM_CH1 (CONFIG_CAL_BASE + 20U)

static float setting_cal_gain_medium_ch0 = 0;
static float setting_cal_gain_medium_ch1 = 0;
static float setting_cal_gain_high_ch0 = 0;
static float setting_cal_gain_high_ch1 = 0;
static float setting_cal_gain_maximum_ch0 = 0;
static float setting_cal_gain_maximum_ch1 = 0;

HAL_StatusTypeDef settings_init()
{
    HAL_StatusTypeDef ret = HAL_OK;

    do {
        log_i("Settings init");
        uint8_t data[128];
        ret = settings_read_buffer(CONFIG_HEADER, data, sizeof(data));
        if (ret != HAL_OK) { break; }

        if (memcmp(data, "DENSITOMETER\0", 13) != 0) {
            log_i("Initializing header");
            memset(data, 0, sizeof(data));
            memcpy(data, "DENSITOMETER\0", 13);
            ret = settings_write_buffer(CONFIG_HEADER, data, sizeof(data));

            /* Initialize Calibration Page */
            settings_write_float(CONFIG_CAL_GAIN_MEDIUM_CH0, NAN);
            settings_write_float(CONFIG_CAL_GAIN_MEDIUM_CH1, NAN);
            settings_write_float(CONFIG_CAL_GAIN_HIGH_CH0, NAN);
            settings_write_float(CONFIG_CAL_GAIN_HIGH_CH1, NAN);
            settings_write_float(CONFIG_CAL_GAIN_MAXIMUM_CH0, NAN);
            settings_write_float(CONFIG_CAL_GAIN_MAXIMUM_CH1, NAN);
        } else {
            /* Load Calibration Page */
            setting_cal_gain_medium_ch0 = (float)(*(__IO uint32_t *)(CONFIG_CAL_GAIN_MEDIUM_CH0));
            setting_cal_gain_medium_ch1 = (float)(*(__IO uint32_t *)(CONFIG_CAL_GAIN_MEDIUM_CH1));
            setting_cal_gain_high_ch0 = (float)(*(__IO uint32_t *)(CONFIG_CAL_GAIN_HIGH_CH0));
            setting_cal_gain_high_ch1 = (float)(*(__IO uint32_t *)(CONFIG_CAL_GAIN_HIGH_CH1));
            setting_cal_gain_maximum_ch0 = (float)(*(__IO uint32_t *)(CONFIG_CAL_GAIN_MAXIMUM_CH0));
            setting_cal_gain_maximum_ch1 = (float)(*(__IO uint32_t *)(CONFIG_CAL_GAIN_MAXIMUM_CH1));
        }
    } while (0);

    return ret;
}

void settings_set_calibration_gain(tsl2591_gain_t gain, float ch0_gain, float ch1_gain)
{
    switch (gain) {
    case TSL2591_GAIN_MEDIUM:
        settings_write_float(CONFIG_CAL_GAIN_MEDIUM_CH0, ch0_gain);
        settings_write_float(CONFIG_CAL_GAIN_MEDIUM_CH1, ch1_gain);
        setting_cal_gain_medium_ch0 = ch0_gain;
        setting_cal_gain_medium_ch1 = ch1_gain;
        break;
    case TSL2591_GAIN_HIGH:
        settings_write_float(CONFIG_CAL_GAIN_HIGH_CH0, ch0_gain);
        settings_write_float(CONFIG_CAL_GAIN_HIGH_CH1, ch1_gain);
        setting_cal_gain_high_ch0 = ch0_gain;
        setting_cal_gain_high_ch1 = ch1_gain;
        break;
    case TSL2591_GAIN_MAXIMUM:
        settings_write_float(CONFIG_CAL_GAIN_MAXIMUM_CH0, ch0_gain);
        settings_write_float(CONFIG_CAL_GAIN_MAXIMUM_CH1, ch1_gain);
        setting_cal_gain_maximum_ch0 = ch0_gain;
        setting_cal_gain_maximum_ch1 = ch1_gain;
        break;
    default:
        break;
    }
}

void settings_get_calibration_gain(tsl2591_gain_t gain, float *ch0_gain, float *ch1_gain)
{
    float ch0_value = 1.0F;
    float ch1_value = 1.0F;

    if (gain == TSL2591_GAIN_MEDIUM) {
        ch0_value = setting_cal_gain_medium_ch0;
        ch1_value = setting_cal_gain_medium_ch1;

        if (ch0_value < TSL2591_GAIN_MEDIUM_MIN || ch0_value > TSL2591_GAIN_MEDIUM_MAX) {
            ch0_value = TSL2591_GAIN_MEDIUM_TYP;
        }
        if (ch1_value < TSL2591_GAIN_MEDIUM_MIN || ch1_value > TSL2591_GAIN_MEDIUM_MAX) {
            ch1_value = TSL2591_GAIN_MEDIUM_TYP;
        }
    } else if (gain == TSL2591_GAIN_HIGH) {
        ch0_value = setting_cal_gain_high_ch0;
        ch1_value = setting_cal_gain_high_ch1;

        if (ch0_value < TSL2591_GAIN_HIGH_MIN || ch0_value > TSL2591_GAIN_HIGH_MAX) {
            ch0_value = TSL2591_GAIN_HIGH_TYP;
        }
        if (ch1_value < TSL2591_GAIN_HIGH_MIN || ch1_value > TSL2591_GAIN_HIGH_MAX) {
            ch1_value = TSL2591_GAIN_HIGH_TYP;
        }
    } else if (gain == TSL2591_GAIN_MAXIMUM) {
        ch0_value = setting_cal_gain_maximum_ch0;
        ch1_value = setting_cal_gain_maximum_ch1;

        if (ch0_value < TSL2591_GAIN_MAXIMUM_CH0_MIN || ch0_value > TSL2591_GAIN_MAXIMUM_CH0_MAX) {
            ch0_value = TSL2591_GAIN_MAXIMUM_CH0_TYP;
        }
        if (ch1_value < TSL2591_GAIN_MAXIMUM_CH1_MIN || ch1_value > TSL2591_GAIN_MAXIMUM_CH1_MAX) {
            ch1_value = TSL2591_GAIN_MAXIMUM_CH1_TYP;
        }
    }

    if (ch0_gain) {
        *ch0_gain = ch0_value;
    }
    if (ch1_gain) {
        *ch1_gain = ch1_value;
    }
}

HAL_StatusTypeDef settings_read_buffer(uint32_t address, uint8_t *data, size_t data_len)
{
    if (!IS_FLASH_DATA_ADDRESS(address)) {
        printf("Invalid EEPROM address\n");
        return HAL_ERROR;
    }
    if (!IS_FLASH_DATA_ADDRESS((address + data_len) - 1)) {
        printf("Invalid length\n");
        return HAL_ERROR;
    }
    if (!data || data_len == 0) {
        printf("Invalid buffer\n");
        return HAL_ERROR;
    }

    for (size_t i = 0; i < data_len; i++) {
        data[i] = *(__IO uint8_t *)(address + i);
    }

    return HAL_OK;
}

HAL_StatusTypeDef settings_write_buffer(uint32_t address, const uint8_t *data, size_t data_len)
{
    HAL_StatusTypeDef ret = HAL_OK;
    if (!IS_FLASH_DATA_ADDRESS(address)) {
        printf("Invalid EEPROM address\n");
        return HAL_ERROR;
    }
    if (!IS_FLASH_DATA_ADDRESS((address + data_len) - 1)) {
        printf("Invalid length\n");
        return HAL_ERROR;
    }
    if (!data || data_len == 0) {
        printf("Invalid buffer\n");
        return HAL_ERROR;
    }

    ret = HAL_FLASHEx_DATAEEPROM_Unlock();
    if (ret != HAL_OK) {
        printf("Unable to unlock EEPROM\n");
        return ret;
    }

    for (size_t i = 0; i < data_len; i++) {
        ret = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, address + i, data[i]);
        if (ret != HAL_OK) {
            printf("EEPROM write error\n");
            break;
        }
    }
    HAL_FLASHEx_DATAEEPROM_Lock();
    return ret;
}

static HAL_StatusTypeDef settings_write_float(uint32_t address, float val)
{
    uint32_t data = (uint32_t)val;
    return settings_write_buffer(address, (uint8_t *)&data, sizeof(data));
}
