#include "state_main_menu.h"

#define LOG_TAG "state_main_menu"

#include <printf.h>
#include <math.h>
#include <string.h>
#include <elog.h>

#include "display.h"
#include "sensor.h"
#include "light.h"
#include "densitometer.h"
#include "settings.h"
#include "util.h"
#include "keypad.h"
#include "tsl2591.h"
#include "task_sensor.h"
#include "sensor.h"
#include "app_descriptor.h"

extern I2C_HandleTypeDef hi2c1;

typedef enum {
    MAIN_MENU_HOME,
    MAIN_MENU_CALIBRATION,
    MAIN_MENU_CALIBRATION_REFLECTION,
    MAIN_MENU_CALIBRATION_TRANSMISSION,
    MAIN_MENU_CALIBRATION_SENSOR_GAIN,
    MAIN_MENU_CALIBRATION_SENSOR_GAIN_RUN,
    MAIN_MENU_CALIBRATION_SENSOR_SLOPE,
    MAIN_MENU_SETTINGS,
    MAIN_MENU_SETTINGS_DIAGNOSTICS,
    MAIN_MENU_ABOUT
} main_menu_state_t;

typedef struct {
    state_t base;
    uint8_t home_option;
    uint8_t cal_option;
    uint8_t cal_sub_option;
    uint8_t settings_option;
    main_menu_state_t menu_state;
} state_main_menu_t;

static void state_main_menu_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state);
static void state_main_menu_process(state_t *state_base, state_controller_t *controller);
static state_main_menu_t state_main_menu_data = {
    .base = {
        .state_entry = state_main_menu_entry,
        .state_process = state_main_menu_process,
        .state_exit = NULL
    },
    .home_option = 1,
    .cal_option = 1,
    .cal_sub_option = 1,
    .settings_option = 1,
    .menu_state = MAIN_MENU_HOME
};

static void main_menu_home(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_reflection(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_transmission(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_sensor_gain(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_sensor_gain_run(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_sensor_slope(state_main_menu_t *state, state_controller_t *controller);
static bool sensor_gain_calibration_callback(sensor_gain_calibration_status_t status, void *user_data);
static bool sensor_calibration_should_abort();
static void main_menu_settings(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_settings_diagnostics(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_about(state_main_menu_t *state, state_controller_t *controller);
static void sensor_read_callback(void *user_data);

#define DENSITY_BUF_SIZE 5

static void format_density_value(char *buf, float value);

state_t *state_main_menu()
{
    return (state_t *)&state_main_menu_data;
}

void state_main_menu_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_main_menu_t *state = (state_main_menu_t *)state_base;
    state->home_option = 1;
    state->cal_option = 1;
    state->cal_sub_option = 1;
    state->settings_option = 1;
    state->menu_state = MAIN_MENU_HOME;
}

void state_main_menu_process(state_t *state_base, state_controller_t *controller)
{
    state_main_menu_t *state = (state_main_menu_t *)state_base;

    sensor_set_light_mode(SENSOR_LIGHT_OFF, false, 0);

    if (state->menu_state == MAIN_MENU_HOME) {
        main_menu_home(state, controller);
    } else if (state->menu_state == MAIN_MENU_CALIBRATION) {
        main_menu_calibration(state, controller);
    } else if (state->menu_state == MAIN_MENU_CALIBRATION_REFLECTION) {
        main_menu_calibration_reflection(state, controller);
    } else if (state->menu_state == MAIN_MENU_CALIBRATION_TRANSMISSION) {
        main_menu_calibration_transmission(state, controller);
    } else if (state->menu_state == MAIN_MENU_CALIBRATION_SENSOR_GAIN) {
        main_menu_calibration_sensor_gain(state, controller);
    } else if (state->menu_state == MAIN_MENU_CALIBRATION_SENSOR_GAIN_RUN) {
        main_menu_calibration_sensor_gain_run(state, controller);
    } else if (state->menu_state == MAIN_MENU_CALIBRATION_SENSOR_SLOPE) {
        main_menu_calibration_sensor_slope(state, controller);
    } else if (state->menu_state == MAIN_MENU_SETTINGS) {
        main_menu_settings(state, controller);
    } else if (state->menu_state == MAIN_MENU_SETTINGS_DIAGNOSTICS) {
        main_menu_settings_diagnostics(state, controller);
    } else if (state->menu_state == MAIN_MENU_ABOUT) {
        main_menu_about(state, controller);
    }
}

void main_menu_home(state_main_menu_t *state, state_controller_t *controller)
{
    log_i("Main Menu");
    state->home_option = display_selection_list(
        "Main Menu", state->home_option,
        "Calibration\n"
        "Settings\n"
        "About");

    if (state->home_option == 1) {
        state->menu_state = MAIN_MENU_CALIBRATION;
    } else if (state->home_option == 2) {
        state->menu_state = MAIN_MENU_SETTINGS;
    } else if (state->home_option == 3) {
        state->menu_state = MAIN_MENU_ABOUT;
    } else {
        state_controller_set_next_state(controller, STATE_HOME);
    }
}

void main_menu_calibration(state_main_menu_t *state, state_controller_t *controller)
{
    state->cal_option = display_selection_list(
        "Calibration", state->cal_option,
        "Reflection\n"
        "Transmission\n"
        "Sensor Gain\n"
        "Sensor Slope");

    if (state->cal_option == 1) {
        state->menu_state = MAIN_MENU_CALIBRATION_REFLECTION;
    } else if (state->cal_option == 2) {
        state->menu_state = MAIN_MENU_CALIBRATION_TRANSMISSION;
    } else if (state->cal_option == 3) {
        state->menu_state = MAIN_MENU_CALIBRATION_SENSOR_GAIN;
    } else if (state->cal_option == 4) {
        state->menu_state = MAIN_MENU_CALIBRATION_SENSOR_SLOPE;
    } else if (state->cal_option == UINT8_MAX) {
        state_controller_set_next_state(controller, STATE_HOME);
    } else {
        state->menu_state = MAIN_MENU_HOME;
        state->cal_option = 1;
    }
}

void main_menu_calibration_reflection(state_main_menu_t *state, state_controller_t *controller)
{
    char buf[128];
    char buf_lo[DENSITY_BUF_SIZE];
    char buf_hi[DENSITY_BUF_SIZE];
    settings_cal_reflection_t cal_reflection;
    uint8_t option = 1;

    sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, LIGHT_REFLECTION_IDLE);

    settings_get_cal_reflection(&cal_reflection);

    do {
        format_density_value(buf_lo, cal_reflection.lo_d);
        format_density_value(buf_hi, cal_reflection.hi_d);

        sprintf_(buf,
            "CAL-LO  [%s]\n"
            "CAL-HI  [%s]\n"
            "** Measure **",
            buf_lo, buf_hi);

        option = display_selection_list(
            "Reflection", option, buf);

        if (option == 1) {
            uint16_t working_value;
            if (is_valid_number(cal_reflection.lo_d) && cal_reflection.lo_d >= 0.0F) {
                working_value = lroundf(cal_reflection.lo_d * 100);
            } else {
                working_value = 0;
            }

            uint8_t input_option = display_input_value_f1_2(
                "CAL-LO (White)\n",
                "D=", &working_value,
                0, 250, NULL);
            if (input_option == 1) {
                cal_reflection.lo_d = working_value / 100.0F;
            } else if (input_option == UINT8_MAX) {
                option = UINT8_MAX;
            }

        } else if (option == 2) {
            uint16_t working_value;
            if (is_valid_number(cal_reflection.hi_d) && cal_reflection.hi_d >= 0.0F) {
                working_value = lroundf(cal_reflection.hi_d * 100);
            } else {
                working_value = 250;
            }

            uint8_t input_option = display_input_value_f1_2(
                "CAL-HI (Black)\n",
                "D=", &working_value,
                0, 250, NULL);
            if (input_option == 1) {
                cal_reflection.hi_d = working_value / 100.0F;
            } else if (input_option == UINT8_MAX) {
                option = UINT8_MAX;
            }

        } else if (option == 3) {
            densitometer_result_t meas_result = DENSITOMETER_OK;
            uint8_t meas_option = 1;
            display_main_elements_t elements = {
                .title = "Calibrating...",
                .mode = DISPLAY_MODE_REFLECTION,
                .density100 = 0,
                .frame = 0
            };

            do {
                /* Validate the target densities, just in case */
                if (!is_valid_number(cal_reflection.lo_d) || !is_valid_number(cal_reflection.hi_d)) {
                    meas_result = DENSITOMETER_CAL_ERROR;
                    break;
                }
                if (cal_reflection.lo_d < 0.00F || cal_reflection.lo_d > REFLECTION_MAX_D) {
                    meas_result = DENSITOMETER_CAL_ERROR;
                    break;
                }
                if (cal_reflection.hi_d < 0.00F || cal_reflection.hi_d > REFLECTION_MAX_D) {
                    meas_result = DENSITOMETER_CAL_ERROR;
                    break;
                }
                if (cal_reflection.lo_d >= cal_reflection.hi_d) {
                    meas_result = DENSITOMETER_CAL_ERROR;
                    break;
                }

                do {
                    meas_option = display_message(
                        "Position\n"
                        "CAL-LO firmly\n"
                        "under sensor",
                        NULL, NULL, " Measure ");
                } while (!keypad_is_detect() && meas_option != 0 && meas_option != UINT8_MAX);

                if (meas_option == 1) {
                    elements.density100 = lroundf(cal_reflection.lo_d * 100);
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate_reflection(&(cal_reflection.lo_value), sensor_read_callback, &elements);
                } else {
                    break;
                }
                if (meas_result != DENSITOMETER_OK) { break; }

                do {
                    meas_option = display_message(
                        "Position\n"
                        "CAL-HI firmly\n"
                        "under sensor",
                        NULL, NULL, " Measure ");
                } while (!keypad_is_detect() && meas_option != 0 && meas_option != UINT8_MAX);

                if (meas_option == 1) {
                    elements.density100 = lroundf(cal_reflection.hi_d * 100);
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate_reflection(&(cal_reflection.hi_value), sensor_read_callback, &elements);
                } else {
                    break;
                }
                if (meas_result != DENSITOMETER_OK) { break; }

                if (!settings_set_cal_reflection(&cal_reflection)) {
                    meas_result = -1;
                }
            } while (0);

            if (meas_option == UINT8_MAX) {
                option = UINT8_MAX;
                break;
            } else if (meas_option == 0) {
                display_message(
                    "Reflection", NULL,
                    "calibration\n"
                    "canceled", " OK ");
                break;
            } else if (meas_result == DENSITOMETER_OK) {
                display_message(
                    "Reflection", NULL,
                    "calibration\n"
                    "complete", " OK ");
                break;
            } else if (meas_result == DENSITOMETER_CAL_ERROR) {
                display_message(
                    "Reflection", NULL,
                    "calibration\n"
                    "values invalid", " OK ");
            } else if (meas_result == DENSITOMETER_SENSOR_ERROR) {
                display_message(
                    "Reflection", NULL,
                    "calibration\n"
                    "failed", " OK ");
            } else if (meas_result == -1) {
                display_message(
                    "Reflection", NULL,
                    "Unable\n"
                    "to save", " OK ");
            }
        }
    } while (option > 0 && option != UINT8_MAX);

    if (option == UINT8_MAX) {
        state_controller_set_next_state(controller, STATE_HOME);
    } else {
        state->menu_state = MAIN_MENU_CALIBRATION;
    }
}

void main_menu_calibration_transmission(state_main_menu_t *state, state_controller_t *controller)
{
    char buf[128];
    char buf_hi[DENSITY_BUF_SIZE];
    settings_cal_transmission_t cal_transmission;
    uint8_t option = 1;

    sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, LIGHT_TRANSMISSION_IDLE);

    settings_get_cal_transmission(&cal_transmission);

    do {
        format_density_value(buf_hi, cal_transmission.hi_d);

        sprintf_(buf,
            "CAL-HI  [%s]\n"
            "** Measure **",
            buf_hi);

        option = display_selection_list(
            "Transmission", option, buf);

        if (option == 1) {
            uint16_t working_value;
            if (is_valid_number(cal_transmission.hi_d) && cal_transmission.hi_d >= 0.0F) {
                working_value = lroundf(cal_transmission.hi_d * 100);
            } else {
                working_value = 300;
            }

            uint8_t input_option = display_input_value_f1_2(
                "CAL-HI\n",
                "D=", &working_value,
                0, 400, NULL);
            if (input_option == 1) {
                cal_transmission.hi_d = working_value / 100.0F;
            } else if (input_option == UINT8_MAX) {
                option = UINT8_MAX;
            }

        } else if (option == 2) {
            densitometer_result_t meas_result = DENSITOMETER_OK;
            uint8_t meas_option = 1;
            display_main_elements_t elements = {
                .title = "Calibrating...",
                .mode = DISPLAY_MODE_TRANSMISSION,
                .density100 = 0,
                .frame = 0
            };

            do {
                /* Validate the target densities, just in case */
                if (!is_valid_number(cal_transmission.hi_d)) {
                    meas_result = DENSITOMETER_CAL_ERROR;
                    break;
                }
                if (cal_transmission.hi_d < 0.00F || cal_transmission.hi_d > TRANSMISSION_MAX_D) {
                    meas_result = DENSITOMETER_CAL_ERROR;
                    break;
                }

                do {
                    meas_option = display_message(
                        "Hold device\n"
                        "firmly closed\n"
                        "with no film",
                        NULL, NULL, " Measure ");
                } while (!keypad_is_detect() && meas_option != 0 && meas_option != UINT8_MAX);

                if (meas_option == 1) {
                    elements.density100 = 0;
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate_transmission(&(cal_transmission.zero_value), sensor_read_callback, &elements);
                } else {
                    break;
                }
                if (meas_result != DENSITOMETER_OK) { break; }

                do {
                    meas_option = display_message(
                        "Position\n"
                        "CAL-HI firmly\n"
                        "under sensor",
                        NULL, NULL, " Measure ");
                } while (!keypad_is_detect() && meas_option != 0 && meas_option != UINT8_MAX);

                if (meas_option == 1) {
                    elements.density100 = lroundf(cal_transmission.hi_d * 100);
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate_transmission(&(cal_transmission.hi_value), sensor_read_callback, &elements);
                } else {
                    break;
                }
                if (meas_result != DENSITOMETER_OK) { break; }

                if (!settings_set_cal_transmission(&cal_transmission)) {
                    meas_result = -1;
                }

            } while (0);

            if (meas_option == UINT8_MAX) {
                option = UINT8_MAX;
                break;
            } else if (meas_option == 0) {
                display_message(
                    "Transmission", NULL,
                    "calibration\n"
                    "canceled", " OK ");
                break;
            } else if (meas_result == DENSITOMETER_OK) {
                display_message(
                    "Transmission", NULL,
                    "calibration\n"
                    "complete", " OK ");
                break;
            } else if (meas_result == DENSITOMETER_CAL_ERROR) {
                display_message(
                    "Transmission", NULL,
                    "calibration\n"
                    "values invalid", " OK ");
            } else if (meas_result == DENSITOMETER_SENSOR_ERROR) {
                display_message(
                    "Transmission", NULL,
                    "calibration\n"
                    "failed", " OK ");
            } else if (meas_result == -1) {
                display_message(
                    "Transmission", NULL,
                    "Unable\n"
                    "to save", " OK ");
            }
        }
    } while (option > 0 && option != UINT8_MAX);

    if (option == UINT8_MAX) {
        state_controller_set_next_state(controller, STATE_HOME);
    } else {
        state->menu_state = MAIN_MENU_CALIBRATION;
    }
}

void main_menu_calibration_sensor_gain(state_main_menu_t *state, state_controller_t *controller)
{
    char buf[192];
    settings_cal_gain_t cal_gain;

    settings_get_cal_gain(&cal_gain);

    sprintf_(buf,
        "L0 = %.1fx\n"
        "L1 = %.1fx\n"
        "M0 = %.4fx\n"
        "M1 = %.4fx\n"
        "H0 = %.3fx\n"
        "H1 = %.3fx\n"
        "X0 = %.2fx\n"
        "X1 = %.2fx\n"
        "* Calibrate *",
        1.0F, 1.0F,
        cal_gain.ch0_medium, cal_gain.ch1_medium,
        cal_gain.ch0_high, cal_gain.ch1_high,
        cal_gain.ch0_maximum, cal_gain.ch1_maximum);

    state->cal_sub_option = display_selection_list(
        "Sensor Gain", state->cal_sub_option,
        buf);

    if (state->cal_sub_option == 9) {
        state->menu_state = MAIN_MENU_CALIBRATION_SENSOR_GAIN_RUN;
    } else if (state->cal_sub_option == UINT8_MAX) {
        state_controller_set_next_state(controller, STATE_HOME);
    } else if (state->cal_sub_option == 0) {
        state->menu_state = MAIN_MENU_CALIBRATION;
        state->cal_sub_option = 1;
    }
}

void main_menu_calibration_sensor_gain_run(state_main_menu_t *state, state_controller_t *controller)
{
    uint8_t option = display_message(
        "Hold device\n"
        "firmly closed\n"
        "with no film", NULL, NULL, " Measure ");

    if (option == 1) {
        osStatus_t ret = osOK;
        if (!keypad_is_detect()) { return; }
        do {
            display_static_list("Sensor Gain", "\nStarting...");
            ret = sensor_gain_calibration(sensor_gain_calibration_callback, NULL);
            if (ret != osOK) { break; }
        } while (0);

        if (ret == osOK) {
            display_message(
                "Sensor", NULL,
                "calibration\n"
                "complete", " OK ");
        } else {
            display_message(
                "Sensor", NULL,
                "calibration\n"
                "failed", " OK ");
        }
        state->menu_state = MAIN_MENU_CALIBRATION;

    } else if (option == UINT8_MAX) {
        state_controller_set_next_state(controller, STATE_HOME);
    } else {
        state->menu_state = MAIN_MENU_CALIBRATION;
    }
}

bool sensor_gain_calibration_callback(sensor_gain_calibration_status_t status, void *user_data)
{
    char buf[128];
    bool update_display = true;
    if (status == SENSOR_GAIN_CALIBRATION_STATUS_MEDIUM) {
        sprintf(buf, "\n"
            "Measuring\n"
            "medium gain");
    } else if (status == SENSOR_GAIN_CALIBRATION_STATUS_HIGH) {
        sprintf(buf, "\n"
            "Measuring\n"
            "high gain");
    } else if (status == SENSOR_GAIN_CALIBRATION_STATUS_MAXIMUM) {
        sprintf(buf, "\n"
            "Measuring\n"
            "maximum gain");
    } else if (status == SENSOR_GAIN_CALIBRATION_STATUS_COOLDOWN) {
        sprintf(buf,
            "Waiting\n"
            "between\n"
            "measurements");
    } else {
        update_display = false;
    }

    if (update_display) {
        display_static_list("Sensor Gain", buf);
    }

    return !sensor_calibration_should_abort();
}

void main_menu_calibration_sensor_slope(state_main_menu_t *state, state_controller_t *controller)
{
    char buf[128];
    settings_cal_slope_t cal_slope;

    if (settings_get_cal_slope(&cal_slope)) {
        sprintf_(buf,
            "B0 = %.6f\n"
            "B1 = %.6f\n"
            "B2 = %.6f",
            cal_slope.b0, cal_slope.b1, cal_slope.b2);

        state->cal_sub_option = display_selection_list(
            "Sensor Slope", state->cal_sub_option,
            buf);
    } else {
        state->cal_sub_option = display_message(
            "Sensor Slope", NULL,
            "calibration\n"
            "not set",
            " OK ");
        if (state->cal_sub_option == 1) {
            state->cal_sub_option = 0;
        }
    }

    if (state->cal_sub_option == UINT8_MAX) {
        state_controller_set_next_state(controller, STATE_HOME);
    } else if (state->cal_sub_option == 0) {
        state->menu_state = MAIN_MENU_CALIBRATION;
        state->cal_sub_option = 1;
    }
}

bool sensor_calibration_should_abort()
{
    keypad_event_t keypad_event;
    if (keypad_wait_for_event(&keypad_event, 0) == osOK) {
        if (keypad_is_key_pressed(&keypad_event, KEYPAD_BUTTON_MENU)) {
            return true;
        } else if(keypad_event.key == KEYPAD_BUTTON_DETECT && !keypad_event.pressed) {
            return true;
        }
    }
    if (!keypad_is_detect()) {
        return true;
    }
    return false;
}

void main_menu_settings(state_main_menu_t *state, state_controller_t *controller)
{
    state->settings_option = display_selection_list(
        "Settings", state->settings_option,
        "Diagnostics");

    if (state->settings_option == 1) {
        state->menu_state = MAIN_MENU_SETTINGS_DIAGNOSTICS;
    } else {
        state->menu_state = MAIN_MENU_HOME;
        state->settings_option = 1;
    }
}

void main_menu_settings_diagnostics(state_main_menu_t *state, state_controller_t *controller)
{
    osStatus_t ret = osOK;
    tsl2591_gain_t gain = TSL2591_GAIN_LOW;
    tsl2591_time_t time = TSL2591_TIME_100MS;
    sensor_reading_t reading;
    float ch0_basic = NAN;
    float ch1_basic = NAN;
    uint8_t light_mode = 0;
    char gain_ch = ' ';
    char light_ch = ' ';
    bool display_mode = false;
    bool config_changed = true;
    bool settings_changed = true;
    char buf[128];

    do {
        ret = sensor_set_config(gain, time);
        if (ret != osOK) { break; }
        ret = sensor_start();
        if (ret != osOK) { break; }

        ret = sensor_get_next_reading(&reading, 2000);
        if (ret != osOK) { break; }
    } while (0);

    if (ret != osOK || reading.gain != gain || reading.time != time) {
        display_message(
            "Sensor", NULL,
            "initialization\n"
            "failed", " OK ");
        state->menu_state = MAIN_MENU_SETTINGS;
        return;
    }

    keypad_event_t keypad_event;
    bool key_changed = false;
    do {
        if (key_changed) {
            key_changed = false;

            if (keypad_is_key_combo_pressed(&keypad_event, KEYPAD_BUTTON_ACTION, KEYPAD_BUTTON_UP)) {
                display_mode = !display_mode;
            } else if (keypad_is_key_pressed(&keypad_event, KEYPAD_BUTTON_ACTION) && !keypad_event.repeated) {
                if (gain < TSL2591_GAIN_MAXIMUM) {
                    gain++;
                } else {
                    gain = TSL2591_GAIN_LOW;
                }
                config_changed = true;
            } else if (keypad_is_key_pressed(&keypad_event, KEYPAD_BUTTON_UP) && !keypad_event.repeated) {
                if (time < TSL2591_TIME_600MS) {
                    time++;
                } else {
                    time = TSL2591_TIME_100MS;
                }
                config_changed = true;
            }

            if (keypad_is_key_pressed(&keypad_event, KEYPAD_BUTTON_DOWN) && !keypad_event.repeated) {
                if (light_mode < 2) {
                    light_mode++;
                } else {
                    light_mode = 0;
                }
                settings_changed = true;
            }

            if (keypad_is_key_pressed(&keypad_event, KEYPAD_BUTTON_MENU)) {
                break;
            } else if (keypad_event.pressed && keypad_event.key == KEYPAD_FORCE_TIMEOUT) {
                state_controller_set_next_state(controller, STATE_HOME);
                break;
            }
        }

        if (config_changed) {
            if (sensor_set_config(gain, time) == osOK) {
                config_changed = false;
                settings_changed = true;
            }
        }

        if (settings_changed) {
            switch (gain) {
            case TSL2591_GAIN_LOW:
                gain_ch = 'L';
                break;
            case TSL2591_GAIN_MEDIUM:
                gain_ch = 'M';
                break;
            case TSL2591_GAIN_HIGH:
                gain_ch = 'H';
                break;
            case TSL2591_GAIN_MAXIMUM:
                gain_ch = 'X';
                break;
            }

            switch (light_mode) {
            case 0:
                sensor_set_light_mode(SENSOR_LIGHT_OFF, false, 0);
                light_ch = '-';
                break;
            case 1:
                sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, 128);
                light_ch = 'R';
                break;
            case 2:
                sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, 128);
                light_ch = 'T';
                break;
            default:
                sensor_set_light_mode(SENSOR_LIGHT_OFF, false, 0);
                light_ch = ' ';
                break;
            }
            settings_changed = false;
        }

        if (sensor_get_next_reading(&reading, 1000) == osOK) {
            bool is_detect = keypad_is_detect();
            if (display_mode) {
                sensor_convert_to_basic_counts(&reading, &ch0_basic, &ch1_basic);
                sprintf_(buf,
                    "CH0=%.5f\n"
                    "CH1=%.5f\n"
                    "[%c][%d][%c][%c]",
                    ch0_basic, ch1_basic,
                    gain_ch, tsl2591_get_time_value_ms(time), light_ch,
                    (is_detect ? '*' : ' '));
            } else {
                sprintf(buf,
                    "CH0=%5d\n"
                    "CH1=%5d\n"
                    "[%c][%d][%c][%c]",
                    reading.ch0_val, reading.ch1_val,
                    gain_ch, tsl2591_get_time_value_ms(time), light_ch,
                    (is_detect ? '*' : ' '));
            }
            display_static_list("Diagnostics", buf);
        }

        if (keypad_wait_for_event(&keypad_event, 100) == osOK) {
            key_changed = true;
        }
    } while (1);

    sensor_stop();

    state->menu_state = MAIN_MENU_SETTINGS;
}

void main_menu_about(state_main_menu_t *state, state_controller_t *controller)
{
    const app_descriptor_t *app_descriptor = app_descriptor_get();
    char buf[128];

    sprintf(buf,
        "Printalyzer\n"
        "Densitometer\n"
        "%s", app_descriptor->version);

    uint8_t option = display_message(buf, NULL, NULL, " OK ");
    if (option == UINT8_MAX) {
        state_controller_set_next_state(controller, STATE_HOME);
    } else {
        state->menu_state = MAIN_MENU_HOME;
    }
}

void sensor_read_callback(void *user_data)
{
    display_main_elements_t *elements = (display_main_elements_t *)user_data;
    elements->frame++;
    if (elements->frame > 2) { elements->frame = 0; }
    display_draw_main_elements(elements);
}

void format_density_value(char *buf, float value)
{
    if (is_valid_number(value) && value >= 0.0F) {
        snprintf_(buf, DENSITY_BUF_SIZE, "%.2f", value);
    } else {
        strncpy(buf, "-.--", DENSITY_BUF_SIZE);
    }
}
