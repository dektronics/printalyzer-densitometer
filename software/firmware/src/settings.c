#include "settings.h"

#define LOG_TAG "settings"

#include <printf.h>
#include <string.h>
#include <math.h>
#include <elog.h>

#include "util.h"

static HAL_StatusTypeDef settings_read_buffer(uint32_t address, uint8_t *data, size_t data_len);
static HAL_StatusTypeDef settings_write_buffer(uint32_t address, const uint8_t *data, size_t data_len);
static float settings_read_float(uint32_t address);
static HAL_StatusTypeDef settings_write_float(uint32_t address, float val);

#define CONFIG_HEADER (DATA_EEPROM_BASE + 0U)

#define CONFIG_CAL_BASE (DATA_EEPROM_BASE + 128U)

#define CONFIG_CAL_GAIN_MEDIUM_CH0  (CONFIG_CAL_BASE + 0U)
#define CONFIG_CAL_GAIN_MEDIUM_CH1  (CONFIG_CAL_BASE + 4U)
#define CONFIG_CAL_GAIN_HIGH_CH0    (CONFIG_CAL_BASE + 8U)
#define CONFIG_CAL_GAIN_HIGH_CH1    (CONFIG_CAL_BASE + 12U)
#define CONFIG_CAL_GAIN_MAXIMUM_CH0 (CONFIG_CAL_BASE + 16U)
#define CONFIG_CAL_GAIN_MAXIMUM_CH1 (CONFIG_CAL_BASE + 20U)

#define CONFIG_CAL_REFLECTION_LO_D     (CONFIG_CAL_BASE + 24U)
#define CONFIG_CAL_REFLECTION_LO_VALUE (CONFIG_CAL_BASE + 28U)
#define CONFIG_CAL_REFLECTION_HI_D     (CONFIG_CAL_BASE + 32U)
#define CONFIG_CAL_REFLECTION_HI_VALUE (CONFIG_CAL_BASE + 36U)

#define CONFIG_CAL_TRANSMISSION_ZERO_VALUE (CONFIG_CAL_BASE + 40U)
#define CONFIG_CAL_TRANSMISSION_HI_D       (CONFIG_CAL_BASE + 44U)
#define CONFIG_CAL_TRANSMISSION_HI_VALUE   (CONFIG_CAL_BASE + 48U)

#define CONFIG_CAL_REFLECTION_LED_FACTOR   (CONFIG_CAL_BASE + 72U)
#define CONFIG_CAL_TRANSMISSION_LED_FACTOR (CONFIG_CAL_BASE + 76U)

#define CONFIG_CAL_SLOPE_B0 (CONFIG_CAL_BASE + 80U)
#define CONFIG_CAL_SLOPE_B1 (CONFIG_CAL_BASE + 84U)
#define CONFIG_CAL_SLOPE_B2 (CONFIG_CAL_BASE + 88U)

static float setting_cal_gain_medium_ch0 = 0;
static float setting_cal_gain_medium_ch1 = 0;
static float setting_cal_gain_high_ch0 = 0;
static float setting_cal_gain_high_ch1 = 0;
static float setting_cal_gain_maximum_ch0 = 0;
static float setting_cal_gain_maximum_ch1 = 0;

static float setting_cal_reflection_lo_d = 0;
static float setting_cal_reflection_lo_value = 0;
static float setting_cal_reflection_hi_d = 0;
static float setting_cal_reflection_hi_value = 0;

static float setting_cal_transmission_zero_value = 0;
static float setting_cal_transmission_hi_d = 0;
static float setting_cal_transmission_hi_value = 0;

static float setting_cal_reflection_led_factor = 0;
static float setting_cal_transmission_led_factor = 0;

static float setting_cal_slope_b0 = 0;
static float setting_cal_slope_b1 = 0;
static float setting_cal_slope_b2 = 0;

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

            settings_write_float(CONFIG_CAL_REFLECTION_LO_D, NAN);
            settings_write_float(CONFIG_CAL_REFLECTION_LO_VALUE, NAN);
            settings_write_float(CONFIG_CAL_REFLECTION_HI_D, NAN);
            settings_write_float(CONFIG_CAL_REFLECTION_HI_VALUE, NAN);

            settings_write_float(CONFIG_CAL_TRANSMISSION_ZERO_VALUE, NAN);
            settings_write_float(CONFIG_CAL_TRANSMISSION_HI_D, NAN);
            settings_write_float(CONFIG_CAL_TRANSMISSION_HI_VALUE, NAN);

            settings_write_float(CONFIG_CAL_REFLECTION_LED_FACTOR, NAN);
            settings_write_float(CONFIG_CAL_TRANSMISSION_LED_FACTOR, NAN);

            settings_write_float(CONFIG_CAL_SLOPE_B0, NAN);
            settings_write_float(CONFIG_CAL_SLOPE_B1, NAN);
            settings_write_float(CONFIG_CAL_SLOPE_B2, NAN);
        } else {
            /* Load Calibration Page */
            setting_cal_gain_medium_ch0 = settings_read_float(CONFIG_CAL_GAIN_MEDIUM_CH0);
            setting_cal_gain_medium_ch1 = settings_read_float(CONFIG_CAL_GAIN_MEDIUM_CH1);
            setting_cal_gain_high_ch0 = settings_read_float(CONFIG_CAL_GAIN_HIGH_CH0);
            setting_cal_gain_high_ch1 = settings_read_float(CONFIG_CAL_GAIN_HIGH_CH1);
            setting_cal_gain_maximum_ch0 = settings_read_float(CONFIG_CAL_GAIN_MAXIMUM_CH0);
            setting_cal_gain_maximum_ch1 = settings_read_float(CONFIG_CAL_GAIN_MAXIMUM_CH1);

            setting_cal_reflection_lo_d = settings_read_float(CONFIG_CAL_REFLECTION_LO_D);
            setting_cal_reflection_lo_value = settings_read_float(CONFIG_CAL_REFLECTION_LO_VALUE);
            setting_cal_reflection_hi_d = settings_read_float(CONFIG_CAL_REFLECTION_HI_D);
            setting_cal_reflection_hi_value = settings_read_float(CONFIG_CAL_REFLECTION_HI_VALUE);

            setting_cal_transmission_zero_value = settings_read_float(CONFIG_CAL_TRANSMISSION_ZERO_VALUE);
            setting_cal_transmission_hi_d = settings_read_float(CONFIG_CAL_TRANSMISSION_HI_D);
            setting_cal_transmission_hi_value = settings_read_float(CONFIG_CAL_TRANSMISSION_HI_VALUE);

            setting_cal_reflection_led_factor = settings_read_float(CONFIG_CAL_REFLECTION_LED_FACTOR);
            setting_cal_transmission_led_factor = settings_read_float(CONFIG_CAL_TRANSMISSION_LED_FACTOR);

            setting_cal_slope_b0 = settings_read_float(CONFIG_CAL_SLOPE_B0);
            setting_cal_slope_b1 = settings_read_float(CONFIG_CAL_SLOPE_B1);
            setting_cal_slope_b2 = settings_read_float(CONFIG_CAL_SLOPE_B2);
        }
    } while (0);

    return ret;
}

void settings_set_cal_gain(tsl2591_gain_t gain, float ch0_gain, float ch1_gain)
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

void settings_get_cal_gain(tsl2591_gain_t gain, float *ch0_gain, float *ch1_gain)
{
    float ch0_value = 1.0F;
    float ch1_value = 1.0F;

    if (gain == TSL2591_GAIN_LOW) {
        ch0_value = 1.0F;
        ch1_value = 1.0F;
    } else if (gain == TSL2591_GAIN_MEDIUM) {
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

void settings_set_cal_reflection_led_factor(float value)
{
    if (settings_write_float(CONFIG_CAL_REFLECTION_LED_FACTOR, value) == HAL_OK) {
        setting_cal_reflection_led_factor = value;
    }
}

void settings_get_cal_reflection_led_factor(float *value)
{
    if (value) {
        *value = setting_cal_reflection_led_factor;
    }
}

void settings_set_cal_transmission_led_factor(float value)
{
    if (settings_write_float(CONFIG_CAL_TRANSMISSION_LED_FACTOR, value) == HAL_OK) {
        setting_cal_transmission_led_factor = value;
    }
}

void settings_get_cal_transmission_led_factor(float *value)
{
    if (value) {
        *value = setting_cal_transmission_led_factor;
    }
}

void settings_set_cal_slope(float b0, float b1, float b2)
{
    if (settings_write_float(CONFIG_CAL_SLOPE_B0, b0) == HAL_OK) {
        setting_cal_slope_b0 = b0;
    }
    if (settings_write_float(CONFIG_CAL_SLOPE_B1, b1) == HAL_OK) {
        setting_cal_slope_b1 = b1;
    }
    if (settings_write_float(CONFIG_CAL_SLOPE_B2, b2) == HAL_OK) {
        setting_cal_slope_b2 = b2;
    }
}

void settings_get_cal_slope(float *b0, float *b1, float *b2)
{
    if (b0) { *b0 = setting_cal_slope_b0; }
    if (b1) { *b1 = setting_cal_slope_b1; }
    if (b2) { *b2 = setting_cal_slope_b2; }
}

void settings_set_cal_reflection_lo(float d, float value)
{
    if (settings_write_float(CONFIG_CAL_REFLECTION_LO_D, d) == HAL_OK) {
        setting_cal_reflection_lo_d = d;
    }
    if (settings_write_float(CONFIG_CAL_REFLECTION_LO_VALUE, value) == HAL_OK) {
        setting_cal_reflection_lo_value = value;
    }
}

void settings_get_cal_reflection_lo(float *d, float *value)
{
    if (d) {
        *d = setting_cal_reflection_lo_d;
    }
    if (value) {
        *value = setting_cal_reflection_lo_value;
    }
}

void settings_set_cal_reflection_hi(float d, float value)
{
    if (settings_write_float(CONFIG_CAL_REFLECTION_HI_D, d) == HAL_OK) {
        setting_cal_reflection_hi_d = d;
    }
    if (settings_write_float(CONFIG_CAL_REFLECTION_HI_VALUE, value) == HAL_OK) {
        setting_cal_reflection_hi_value = value;
    }
}

void settings_get_cal_reflection_hi(float *d, float *value)
{
    if (d) {
        *d = setting_cal_reflection_hi_d;
    }
    if (value) {
        *value = setting_cal_reflection_hi_value;
    }
}

void settings_set_cal_transmission_zero(float value)
{
    if (settings_write_float(CONFIG_CAL_TRANSMISSION_ZERO_VALUE, value) == HAL_OK) {
        setting_cal_transmission_zero_value = value;
    }
}

void settings_get_cal_transmission_zero(float *value)
{
    if (value) {
        *value = setting_cal_transmission_zero_value;
    }
}

void settings_set_cal_transmission_hi(float d, float value)
{
    if (settings_write_float(CONFIG_CAL_TRANSMISSION_HI_D, d) == HAL_OK) {
        setting_cal_transmission_hi_d = d;
    }
    if (settings_write_float(CONFIG_CAL_TRANSMISSION_HI_VALUE, value) == HAL_OK) {
        setting_cal_transmission_hi_value = value;
    }
}

void settings_get_cal_transmission_hi(float *d, float *value)
{
    if (d) {
        *d = setting_cal_transmission_hi_d;
    }
    if (value) {
        *value = setting_cal_transmission_hi_value;
    }
}

HAL_StatusTypeDef settings_read_buffer(uint32_t address, uint8_t *data, size_t data_len)
{
    if (!IS_FLASH_DATA_ADDRESS(address)) {
        log_e("Invalid EEPROM address");
        return HAL_ERROR;
    }
    if (!IS_FLASH_DATA_ADDRESS((address + data_len) - 1)) {
        log_e("Invalid length");
        return HAL_ERROR;
    }
    if (!data || data_len == 0) {
        log_e("Invalid buffer");
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
        log_e("Invalid EEPROM address");
        return HAL_ERROR;
    }
    if (!IS_FLASH_DATA_ADDRESS((address + data_len) - 1)) {
        log_e("Invalid length");
        return HAL_ERROR;
    }
    if (!data || data_len == 0) {
        log_e("Invalid buffer");
        return HAL_ERROR;
    }

    ret = HAL_FLASHEx_DATAEEPROM_Unlock();
    if (ret != HAL_OK) {
        log_e("Unable to unlock EEPROM: %d", ret);
        return ret;
    }

    for (size_t i = 0; i < data_len; i++) {
        ret = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, address + i, data[i]);
        if (ret != HAL_OK) {
            log_e("EEPROM write error: %d [%d]", ret, i);
            log_e("FLASH last error: %d", HAL_FLASH_GetError());
            break;
        }
    }
    HAL_FLASHEx_DATAEEPROM_Lock();
    return ret;
}

float settings_read_float(uint32_t address)
{
    __IO uint32_t *data = (__IO uint32_t *)(address);
    return copy_to_f32((uint8_t *)data);
}

HAL_StatusTypeDef settings_write_float(uint32_t address, float val)
{
    uint8_t data[4];
    copy_from_f32(data, val);
    return settings_write_buffer(address, data, sizeof(data));
}
