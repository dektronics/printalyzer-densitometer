#include "tsl2591.h"

#define LOG_TAG "tsl2591"

#include "stm32l0xx_hal.h"

#include <stdio.h>
#include <elog.h>

/* I2C device address */
static const uint8_t TSL2591_ADDRESS = 0x29 << 1; // Use 8-bit address

/* Registers */
#define TSL2591_ENABLE  0x00 /*!< Enables states and interrupts */
#define TSL2591_CONFIG  0x01 /*!< ALS gain and integration time configuration */
#define TSL2591_AILTL   0x04 /*!< ALS interrupt low threshold low byte */
#define TSL2591_AILTH   0x05 /*!< ALS interrupt low threshold high byte */
#define TSL2591_AIHTL   0x06 /*!< ALS interrupt high threshold low byte */
#define TSL2591_AIHTH   0x07 /*!< ALS interrupt high threshold high byte */
#define TSL2591_NPAILTL 0x08 /*!< No Persist ALS interrupt low threshold low byte */
#define TSL2591_NPAILTH 0x09 /*!< No Persist ALS interrupt low threshold high byte */
#define TSL2591_NPAIHTL 0x0A /*!< No Persist ALS interrupt high threshold low byte */
#define TSL2591_NPAIHTH 0x0B /*!< No Persist ALS interrupt high threshold high byte */
#define TSL2591_PERSIST 0x0C /*!< Interrupt persistence filter */
#define TSL2591_PID     0x11 /*!< Package */
#define TSL2591_ID      0x12 /*!< Device ID */
#define TSL2591_STATUS  0x13 /*!< Device status */
#define TSL2591_C0DATAL 0x14 /*!< CH0 ADC low data byte */
#define TSL2591_C0DATAH 0x15 /*!< CH0 ADC high data byte */
#define TSL2591_C1DATAL 0x16 /*!< CH1 ADC low data byte */
#define TSL2591_C1DATAH 0x17 /*!< CH1 ADC high data byte */

/* Command Register Values */
#define TSL2591_CMD_NORMAL                   0xA0 /*!< Normal operation */
#define TSL2591_CMD_INT_FORCE                0xE4 /*!< Interrupt set - forces an interrupt */
#define TSL2591_CMD_INT_CLEAR_ALS            0xE6 /*!< Clears ALS interrupt */
#define TSL2591_CMD_INT_CLEAR_ALS_NO_PERSIST 0xE7 /*!< Clears ALS and no persist ALS interrupt */
#define TSL2591_CMD_INT_CLEAR_NO_PERSIST     0xEA /*!< Clears no persist ALS interrupt */

/* Enable Register Values */
#define TSL2591_ENABLE_NPIEN 0x80 /*!< No persist interrupt enable */
#define TSL2591_ENABLE_SAI   0x40 /*!< Sleep after interrupt */
#define TSL2591_ENABLE_AIEN  0x10 /*!< ALS interrupt enable */
#define TSL2591_ENABLE_AEN   0x02 /*!< ALS enable */
#define TSL2591_ENABLE_PON   0x01 /*!< Power ON */

HAL_StatusTypeDef tsl2591_init(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef ret;
    uint8_t data;

    log_i("Initializing TSL2591");

    ret = HAL_I2C_Mem_Read(hi2c, TSL2591_ADDRESS,
        TSL2591_CMD_NORMAL | TSL2591_PID, I2C_MEMADD_SIZE_8BIT,
        &data, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    log_i("Package: %02X", data);

    ret = HAL_I2C_Mem_Read(hi2c, TSL2591_ADDRESS,
        TSL2591_CMD_NORMAL | TSL2591_ID, I2C_MEMADD_SIZE_8BIT,
        &data, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    log_i("Device ID: %02X", data);

    if (data != 0x50) {
        log_e("Invalid Device ID");
        return HAL_ERROR;
    }

    ret = HAL_I2C_Mem_Read(hi2c, TSL2591_ADDRESS,
        TSL2591_CMD_NORMAL | TSL2591_STATUS, I2C_MEMADD_SIZE_8BIT,
        &data, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    log_i("Status: %02X", data);

    /* Power on the sensor */
    ret = tsl2591_enable(hi2c);
    if (ret != HAL_OK) {
        return ret;
    }

    /* Set default integration and gain */
    ret = tsl2591_set_config(hi2c, TSL2591_GAIN_HIGH, TSL2591_TIME_300MS);
    if (ret != HAL_OK) {
        return ret;
    }

    /* Power off the sensor */
    ret = tsl2591_disable(hi2c);
    if (ret != HAL_OK) {
        return ret;
    }

    log_i("TSL2591 Initialized");

    return HAL_OK;
}

HAL_StatusTypeDef tsl2591_enable(I2C_HandleTypeDef *hi2c)
{
    uint8_t data = TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN;
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(hi2c, TSL2591_ADDRESS,
        TSL2591_CMD_NORMAL | TSL2591_ENABLE, I2C_MEMADD_SIZE_8BIT,
        &data, 1, HAL_MAX_DELAY);
    return ret;
}

HAL_StatusTypeDef tsl2591_disable(I2C_HandleTypeDef *hi2c)
{
    uint8_t data = 0x00;
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(hi2c, TSL2591_ADDRESS,
        TSL2591_CMD_NORMAL | TSL2591_ENABLE, I2C_MEMADD_SIZE_8BIT,
        &data, 1, HAL_MAX_DELAY);
    return ret;
}

HAL_StatusTypeDef tsl2591_set_config(I2C_HandleTypeDef *hi2c, tsl2591_gain_t gain, tsl2591_time_t time)
{
    if (gain < TSL2591_GAIN_LOW || gain > TSL2591_GAIN_MAXIMUM) {
        return HAL_ERROR;
    }
    if (time < TSL2591_TIME_100MS || time > TSL2591_TIME_600MS) {
        return HAL_ERROR;
    }

    uint8_t data = ((uint8_t)(gain & 0x03) << 4) | ((uint8_t)(time & 0x07));
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(hi2c, TSL2591_ADDRESS,
        TSL2591_CMD_NORMAL | TSL2591_CONFIG, I2C_MEMADD_SIZE_8BIT,
        &data, 1, HAL_MAX_DELAY);
    return ret;
}

HAL_StatusTypeDef tsl2591_get_config(I2C_HandleTypeDef *hi2c, tsl2591_gain_t *gain, tsl2591_time_t *time)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t data;

    ret = HAL_I2C_Mem_Read(hi2c, TSL2591_ADDRESS,
        TSL2591_CMD_NORMAL | TSL2591_CONFIG, I2C_MEMADD_SIZE_8BIT,
        &data, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        log_e("i2c_read_register error: %d", ret);
        return ret;
    }

    if (gain) {
        *gain = (data & 0x30) >> 4;
    }
    if (time) {
        *time = data & 0x07;
    }

    return ret;
}

HAL_StatusTypeDef tsl2591_get_status_valid(I2C_HandleTypeDef *hi2c, bool *valid)
{
    if (!valid) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t data;

    ret = HAL_I2C_Mem_Read(hi2c, TSL2591_ADDRESS,
        TSL2591_CMD_NORMAL | TSL2591_STATUS, I2C_MEMADD_SIZE_8BIT,
        &data, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        log_e("i2c_read_register error: %d", ret);
        return ret;
    }

    *valid = (data & 0x01) ? true : false;

    return ret;
}

HAL_StatusTypeDef tsl2591_get_full_channel_data(I2C_HandleTypeDef *hi2c, uint16_t *ch0_val, uint16_t *ch1_val)
{
    HAL_StatusTypeDef ret;
    uint8_t data[4];

    ret = HAL_I2C_Mem_Read(hi2c, TSL2591_ADDRESS,
        TSL2591_CMD_NORMAL | TSL2591_C0DATAL, I2C_MEMADD_SIZE_8BIT,
        data, sizeof(data), HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    /* Channel 0 - visible + infrared */
    if (ch0_val) {
        *ch0_val = data[0] | data[1] << 8;
    }

    /* Channel 1 - infrared only */
    if (ch1_val) {
        *ch1_val = data[2] | data[3] << 8;
    }

    return HAL_OK;
}
