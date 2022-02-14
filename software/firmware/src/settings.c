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

#define CONFIG_CAL_SLOPE_B0 (CONFIG_CAL_BASE + 80U)
#define CONFIG_CAL_SLOPE_B1 (CONFIG_CAL_BASE + 84U)
#define CONFIG_CAL_SLOPE_B2 (CONFIG_CAL_BASE + 88U)

static settings_cal_gain_t setting_cal_gain = {0};
static settings_cal_reflection_t setting_cal_reflection = {0};
static settings_cal_transmission_t setting_cal_transmission = {0};
static settings_cal_slope_t setting_cal_slope = {0};

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

            settings_write_float(CONFIG_CAL_SLOPE_B0, NAN);
            settings_write_float(CONFIG_CAL_SLOPE_B1, NAN);
            settings_write_float(CONFIG_CAL_SLOPE_B2, NAN);

            settings_write_float(CONFIG_CAL_REFLECTION_LO_D, NAN);
            settings_write_float(CONFIG_CAL_REFLECTION_LO_VALUE, NAN);
            settings_write_float(CONFIG_CAL_REFLECTION_HI_D, NAN);
            settings_write_float(CONFIG_CAL_REFLECTION_HI_VALUE, NAN);

            settings_write_float(CONFIG_CAL_TRANSMISSION_ZERO_VALUE, NAN);
            settings_write_float(CONFIG_CAL_TRANSMISSION_HI_D, NAN);
            settings_write_float(CONFIG_CAL_TRANSMISSION_HI_VALUE, NAN);
        } else {
            /* Load Calibration Page */
            setting_cal_gain.ch0_medium = settings_read_float(CONFIG_CAL_GAIN_MEDIUM_CH0);
            setting_cal_gain.ch1_medium = settings_read_float(CONFIG_CAL_GAIN_MEDIUM_CH1);
            setting_cal_gain.ch0_high = settings_read_float(CONFIG_CAL_GAIN_HIGH_CH0);
            setting_cal_gain.ch1_high = settings_read_float(CONFIG_CAL_GAIN_HIGH_CH1);
            setting_cal_gain.ch0_maximum = settings_read_float(CONFIG_CAL_GAIN_MAXIMUM_CH0);
            setting_cal_gain.ch1_maximum = settings_read_float(CONFIG_CAL_GAIN_MAXIMUM_CH1);

            setting_cal_slope.b0 = settings_read_float(CONFIG_CAL_SLOPE_B0);
            setting_cal_slope.b1 = settings_read_float(CONFIG_CAL_SLOPE_B1);
            setting_cal_slope.b2 = settings_read_float(CONFIG_CAL_SLOPE_B2);

            setting_cal_reflection.lo_d = settings_read_float(CONFIG_CAL_REFLECTION_LO_D);
            setting_cal_reflection.lo_value = settings_read_float(CONFIG_CAL_REFLECTION_LO_VALUE);
            setting_cal_reflection.hi_d = settings_read_float(CONFIG_CAL_REFLECTION_HI_D);
            setting_cal_reflection.hi_value = settings_read_float(CONFIG_CAL_REFLECTION_HI_VALUE);

            setting_cal_transmission.zero_value = settings_read_float(CONFIG_CAL_TRANSMISSION_ZERO_VALUE);
            setting_cal_transmission.hi_d = settings_read_float(CONFIG_CAL_TRANSMISSION_HI_D);
            setting_cal_transmission.hi_value = settings_read_float(CONFIG_CAL_TRANSMISSION_HI_VALUE);
        }
    } while (0);

    return ret;
}

bool settings_set_cal_gain(const settings_cal_gain_t *cal_gain)
{
    HAL_StatusTypeDef ret = HAL_OK;
    if (!cal_gain) { return false; }

    do {
        ret = settings_write_float(CONFIG_CAL_GAIN_MEDIUM_CH0, cal_gain->ch0_medium);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_GAIN_MEDIUM_CH1, cal_gain->ch1_medium);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_GAIN_HIGH_CH0, cal_gain->ch0_high);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_GAIN_HIGH_CH1, cal_gain->ch1_high);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_GAIN_MAXIMUM_CH0, cal_gain->ch0_maximum);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_GAIN_MAXIMUM_CH1, cal_gain->ch1_maximum);
        if (ret != HAL_OK) { break; }
    } while (0);

    if (ret == HAL_OK) {
        memcpy(&setting_cal_gain, cal_gain, sizeof(settings_cal_gain_t));
        return true;
    } else {
        return false;
    }
}

bool settings_get_cal_gain(settings_cal_gain_t *cal_gain)
{
    bool valid = true;
    if (!cal_gain) { return false; }

    /* Copy over the settings values */
    memcpy(cal_gain, &setting_cal_gain, sizeof(settings_cal_gain_t));

    /* Validate the complete set */
    do {
        if (cal_gain->ch0_medium < TSL2591_GAIN_MEDIUM_MIN || cal_gain->ch0_medium > TSL2591_GAIN_MEDIUM_MAX) {
            valid = false;
            break;
        }
        if (cal_gain->ch1_medium < TSL2591_GAIN_MEDIUM_MIN || cal_gain->ch1_medium > TSL2591_GAIN_MEDIUM_MAX) {
            valid = false;
            break;
        }
        if (cal_gain->ch0_high < TSL2591_GAIN_HIGH_MIN || cal_gain->ch0_high > TSL2591_GAIN_HIGH_MAX) {
            valid = false;
            break;
        }
        if (cal_gain->ch1_high < TSL2591_GAIN_HIGH_MIN || cal_gain->ch1_high > TSL2591_GAIN_HIGH_MAX) {
            valid = false;
            break;
        }
        if (cal_gain->ch0_maximum < TSL2591_GAIN_MAXIMUM_CH0_MIN || cal_gain->ch0_maximum > TSL2591_GAIN_MAXIMUM_CH0_MAX) {
            valid = false;
            break;
        }
        if (cal_gain->ch1_maximum < TSL2591_GAIN_MAXIMUM_CH1_MIN || cal_gain->ch1_maximum > TSL2591_GAIN_MAXIMUM_CH1_MAX) {
            valid = false;
            break;
        }
    } while (0);

    /* Set default values if validation fails */
    if (!valid) {
        cal_gain->ch0_medium = TSL2591_GAIN_MEDIUM_TYP;
        cal_gain->ch1_medium = TSL2591_GAIN_MEDIUM_TYP;
        cal_gain->ch0_high = TSL2591_GAIN_HIGH_TYP;
        cal_gain->ch1_high = TSL2591_GAIN_HIGH_TYP;
        cal_gain->ch0_maximum = TSL2591_GAIN_MAXIMUM_CH0_TYP;
        cal_gain->ch1_maximum = TSL2591_GAIN_MAXIMUM_CH1_TYP;
        return false;
    } else {
        return true;
    }
}

void settings_get_cal_gain_fields(const settings_cal_gain_t *cal_gain, tsl2591_gain_t gain, float *ch0_gain, float *ch1_gain)
{
    float ch0_value = 1.0F;
    float ch1_value = 1.0F;

    if (!cal_gain) { return; }

    if (gain == TSL2591_GAIN_LOW) {
        ch0_value = 1.0F;
        ch1_value = 1.0F;
    } else if (gain == TSL2591_GAIN_MEDIUM) {
        ch0_value = cal_gain->ch0_medium;
        ch1_value = cal_gain->ch1_medium;
    } else if (gain == TSL2591_GAIN_HIGH) {
        ch0_value = cal_gain->ch0_high;
        ch1_value = cal_gain->ch1_high;
    } else if (gain == TSL2591_GAIN_MAXIMUM) {
        ch0_value = cal_gain->ch0_maximum;
        ch1_value = cal_gain->ch1_maximum;
    }

    if (ch0_gain) {
        *ch0_gain = ch0_value;
    }
    if (ch1_gain) {
        *ch1_gain = ch1_value;
    }
}

bool settings_set_cal_slope(const settings_cal_slope_t *cal_slope)
{
    HAL_StatusTypeDef ret = HAL_OK;
    if (!cal_slope) { return false; }

    do {
        ret = settings_write_float(CONFIG_CAL_SLOPE_B0, cal_slope->b0);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_SLOPE_B1, cal_slope->b1);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_SLOPE_B2, cal_slope->b2);
        if (ret != HAL_OK) { break; }
    } while (0);

    if (ret == HAL_OK) {
        memcpy(&setting_cal_slope, cal_slope, sizeof(settings_cal_slope_t));
        return true;
    } else {
        return false;
    }
}

bool settings_get_cal_slope(settings_cal_slope_t *cal_slope)
{
    bool valid = true;
    if (!cal_slope) { return false; }

    /* Copy over the settings values */
    memcpy(cal_slope, &setting_cal_slope, sizeof(settings_cal_slope_t));

    /* Validate the complete set */
    do {
        if (isnanf(cal_slope->b0) || isinff(cal_slope->b0)) {
            valid = false;
            break;
        }
        if (isnanf(cal_slope->b1) || isinff(cal_slope->b1)) {
            valid = false;
            break;
        }
        if (isnanf(cal_slope->b2) || isinff(cal_slope->b2)) {
            valid = false;
            break;
        }
    } while (0);

    /* Set default values if validation fails */
    if (!valid) {
        cal_slope->b0 = NAN;
        cal_slope->b1 = NAN;
        cal_slope->b2 = NAN;
        return false;
    } else {
        return true;
    }
}

bool settings_set_cal_reflection(const settings_cal_reflection_t *cal_reflection)
{
    HAL_StatusTypeDef ret = HAL_OK;
    if (!cal_reflection) { return false; }

    do {
        ret = settings_write_float(CONFIG_CAL_REFLECTION_LO_D, cal_reflection->lo_d);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_REFLECTION_LO_VALUE, cal_reflection->lo_value);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_REFLECTION_HI_D, cal_reflection->hi_d);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_REFLECTION_HI_VALUE, cal_reflection->hi_value);
        if (ret != HAL_OK) { break; }
    } while (0);

    if (ret == HAL_OK) {
        memcpy(&setting_cal_reflection, cal_reflection, sizeof(settings_cal_reflection_t));
        return true;
    } else {
        return false;
    }
}

bool settings_get_cal_reflection(settings_cal_reflection_t *cal_reflection)
{
    bool valid = true;
    if (!cal_reflection) { return false; }

    /* Copy over the settings values */
    memcpy(cal_reflection, &setting_cal_reflection, sizeof(settings_cal_reflection_t));

    /* Validate the complete set */
    do {
        if (isnanf(cal_reflection->lo_d) || isinff(cal_reflection->lo_d)) {
            valid = false;
            break;
        }
        if (isnanf(cal_reflection->lo_value) || isinff(cal_reflection->lo_value)) {
            valid = false;
            break;
        }
        if (isnanf(cal_reflection->hi_d) || isinff(cal_reflection->hi_d)) {
            valid = false;
            break;
        }
        if (isnanf(cal_reflection->hi_value) || isinff(cal_reflection->hi_value)) {
            valid = false;
            break;
        }
    } while (0);

    /* Set default values if validation fails */
    if (!valid) {
        cal_reflection->lo_d = NAN;
        cal_reflection->lo_value = NAN;
        cal_reflection->hi_d = NAN;
        cal_reflection->hi_value = NAN;
        return false;
    } else {
        return true;
    }
}

bool settings_set_cal_transmission(const settings_cal_transmission_t *cal_transmission)
{
    HAL_StatusTypeDef ret = HAL_OK;
    if (!cal_transmission) { return false; }

    do {
        ret = settings_write_float(CONFIG_CAL_TRANSMISSION_ZERO_VALUE, cal_transmission->zero_value);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_TRANSMISSION_HI_D, cal_transmission->hi_d);
        if (ret != HAL_OK) { break; }

        ret = settings_write_float(CONFIG_CAL_TRANSMISSION_HI_VALUE, cal_transmission->hi_value);
        if (ret != HAL_OK) { break; }
    } while (0);

    if (ret == HAL_OK) {
        memcpy(&setting_cal_transmission, cal_transmission, sizeof(settings_cal_transmission_t));
        return true;
    } else {
        return false;
    }
}

bool settings_get_cal_transmission(settings_cal_transmission_t *cal_transmission)
{
    bool valid = true;
    if (!cal_transmission) { return false; }

    /* Copy over the settings values */
    memcpy(cal_transmission, &setting_cal_transmission, sizeof(settings_cal_transmission_t));

    /* Validate the complete set */
    do {
        if (isnanf(cal_transmission->zero_value) || isinff(cal_transmission->zero_value)) {
            valid = false;
            break;
        }
        if (isnanf(cal_transmission->hi_d) || isinff(cal_transmission->hi_d)) {
            valid = false;
            break;
        }
        if (isnanf(cal_transmission->hi_value) || isinff(cal_transmission->hi_value)) {
            valid = false;
            break;
        }
    } while (0);

    /* Set default values if validation fails */
    if (!valid) {
        cal_transmission->zero_value = NAN;
        cal_transmission->hi_d = NAN;
        cal_transmission->hi_value = NAN;
        return false;
    } else {
        return true;
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
