#ifndef SETTINGS_H
#define SETTINGS_H

#include "stm32l0xx_hal.h"

#include <stdbool.h>

#include "tsl2591.h"

typedef struct {
    float ch0_medium;
    float ch1_medium;
    float ch0_high;
    float ch1_high;
    float ch0_maximum;
    float ch1_maximum;
} settings_cal_gain_t;

typedef struct {
    float b0;
    float b1;
    float b2;
} settings_cal_slope_t;

typedef struct {
    float lo_d;
    float lo_value;
    float hi_d;
    float hi_value;
} settings_cal_reflection_t;

typedef struct {
    float zero_value;
    float hi_d;
    float hi_value;
} settings_cal_transmission_t;

HAL_StatusTypeDef settings_init();

HAL_StatusTypeDef settings_wipe();

/**
 * Set the gain calibration values.
 *
 * @param cal_gain Struct populated with values to save
 * @return True if saved, false on error
 */
bool settings_set_cal_gain(const settings_cal_gain_t *cal_gain);

/**
 * Get the gain calibration values.
 * If a valid set of values are not available, but the provided struct is
 * usable, default values will be returned.
 *
 * @param cal_gain Struct to be populated with saved values
 * @return True if valid values are returned, false otherwise.
 */
bool settings_get_cal_gain(settings_cal_gain_t *cal_gain);

/**
 * Convenience function to get gain calibration fields for a particular
 * gain setting.
 */
void settings_get_cal_gain_fields(const settings_cal_gain_t *cal_gain, tsl2591_gain_t gain, float *ch0_gain, float *ch1_gain);

/**
 * Check if the gain calibration values are valid
 *
 * @param cal_gain Struct to validate
 * @return True if valid, false if invalid
 */
bool settings_validate_cal_gain(const settings_cal_gain_t *cal_gain);

/**
 * Set the slope calibration values.
 *
 * @param cal_slope Struct populated with values to save
 * @return True if saved, false on error
 */
bool settings_set_cal_slope(const settings_cal_slope_t *cal_slope);

/**
 * Get the slope calibration values.
 * If a valid set of values are not available, but the provided struct is
 * usable, it will be initialized to NaN.
 *
 * @param cal_slope Struct to be populated with saved values
 * @return True if valid values are returned, false otherwise.
 */
bool settings_get_cal_slope(settings_cal_slope_t *cal_slope);

/**
 * Check if the slope calibration values are valid
 *
 * @param cal_slope Struct to validate
 * @return True if valid, false if invalid
 */
bool settings_validate_cal_slope(const settings_cal_slope_t *cal_slope);

/**
 * Set the reflection density calibration values.
 *
 * @param cal_reflection Struct populated with values to save
 * @return True if saved, false on error
 */
bool settings_set_cal_reflection(const settings_cal_reflection_t *cal_reflection);

/**
 * Get the reflection density calibration values.
 * If a valid set of values are not available, but the provided struct is
 * usable, it will be initialized to NaN.
 *
 * @param cal_reflection Struct to be populated with saved values
 * @return True if valid values are returned, false otherwise.
 */
bool settings_get_cal_reflection(settings_cal_reflection_t *cal_reflection);

/**
 * Check if the reflection calibration values are valid
 *
 * @param cal_reflection Struct to validate
 * @return True if valid, false if invalid
 */
bool settings_validate_cal_reflection(const settings_cal_reflection_t *cal_reflection);

/**
 * Set the transmission density calibration values.
 *
 * @param cal_transmission Struct populated with values to save
 * @return True if saved, false on error
 */
bool settings_set_cal_transmission(const settings_cal_transmission_t *cal_transmission);

/**
 * Get the transmission density calibration values.
 * If a valid set of values are not available, but the provided struct is
 * usable, it will be initialized to NaN.
 *
 * @param cal_transmission Struct to be populated with saved values
 * @return True if valid values are returned, false otherwise.
 */
bool settings_get_cal_transmission(settings_cal_transmission_t *cal_transmission);

/**
 * Check if the transmission calibration values are valid
 *
 * @param cal_transmission Struct to validate
 * @return True if valid, false if invalid
 */
bool settings_validate_cal_transmission(const settings_cal_transmission_t *cal_transmission);

#endif /* SETTINGS_H */
