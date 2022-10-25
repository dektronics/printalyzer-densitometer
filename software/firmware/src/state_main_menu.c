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
#include "task_usbd.h"
#include "sensor.h"
#include "app_descriptor.h"

extern I2C_HandleTypeDef hi2c1;

typedef enum {
    MAIN_MENU_HOME,
    MAIN_MENU_CALIBRATION,
    MAIN_MENU_CALIBRATION_REFLECTION,
    MAIN_MENU_CALIBRATION_TRANSMISSION,
    MAIN_MENU_CALIBRATION_SENSOR_GAIN,
    MAIN_MENU_CALIBRATION_SENSOR_SLOPE,
    MAIN_MENU_SETTINGS,
    MAIN_MENU_SETTINGS_IDLE_LIGHT,
    MAIN_MENU_SETTINGS_USB_KEY,
    MAIN_MENU_SETTINGS_DIAGNOSTICS,
    MAIN_MENU_ABOUT
} main_menu_state_t;

typedef struct {
    state_t base;
    uint8_t home_option;
    uint8_t cal_option;
    uint8_t cal_sub_option;
    uint8_t settings_option;
    uint8_t settings_sub_option;
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
    .settings_sub_option = 1,
    .menu_state = MAIN_MENU_HOME
};

static void main_menu_home(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_reflection(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_transmission(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_sensor_gain(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_sensor_slope(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_settings(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_settings_idle_light(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_settings_usb_key(state_main_menu_t *state, state_controller_t *controller);
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
    state->settings_sub_option = 1;
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
    } else if (state->menu_state == MAIN_MENU_CALIBRATION_SENSOR_SLOPE) {
        main_menu_calibration_sensor_slope(state, controller);
    } else if (state->menu_state == MAIN_MENU_SETTINGS) {
        main_menu_settings(state, controller);
    } else if (state->menu_state == MAIN_MENU_SETTINGS_IDLE_LIGHT) {
        main_menu_settings_idle_light(state, controller);
    } else if (state->menu_state == MAIN_MENU_SETTINGS_USB_KEY) {
        main_menu_settings_usb_key(state, controller);
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

                /* Activate the idle light at default brightness */
                sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, SETTING_IDLE_LIGHT_REFL_DEFAULT);

                do {
                    meas_option = display_message(
                        "Position\n"
                        "CAL-LO firmly\n"
                        "under sensor",
                        NULL, NULL, " Measure ");
                } while (!keypad_is_detect() && meas_option != 0 && meas_option != UINT8_MAX);

                if (meas_option == 1) {
                    densitometer_t *densitometer = densitometer_reflection();
                    elements.density100 = lroundf(cal_reflection.lo_d * 100);
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate(densitometer, &(cal_reflection.lo_value), sensor_read_callback, &elements);
                } else {
                    break;
                }
                if (meas_result != DENSITOMETER_OK) { break; }

                /* Activate the idle light at default brightness */
                sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, SETTING_IDLE_LIGHT_REFL_DEFAULT);

                do {
                    meas_option = display_message(
                        "Position\n"
                        "CAL-HI firmly\n"
                        "under sensor",
                        NULL, NULL, " Measure ");
                } while (!keypad_is_detect() && meas_option != 0 && meas_option != UINT8_MAX);

                if (meas_option == 1) {
                    densitometer_t *densitometer = densitometer_reflection();
                    elements.density100 = lroundf(cal_reflection.hi_d * 100);
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate(densitometer, &(cal_reflection.hi_value), sensor_read_callback, &elements);
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

                sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, SETTING_IDLE_LIGHT_TRAN_DEFAULT);

                do {
                    meas_option = display_message(
                        "Hold device\n"
                        "firmly closed\n"
                        "with no film",
                        NULL, NULL, " Measure ");
                } while (!keypad_is_detect() && meas_option != 0 && meas_option != UINT8_MAX);

                if (meas_option == 1) {
                    densitometer_t *densitometer = densitometer_transmission();
                    elements.density100 = 0;
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate(densitometer, &(cal_transmission.zero_value), sensor_read_callback, &elements);
                } else {
                    break;
                }
                if (meas_result != DENSITOMETER_OK) { break; }

                sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, SETTING_IDLE_LIGHT_TRAN_DEFAULT);

                do {
                    meas_option = display_message(
                        "Position\n"
                        "CAL-HI firmly\n"
                        "under sensor",
                        NULL, NULL, " Measure ");
                } while (!keypad_is_detect() && meas_option != 0 && meas_option != UINT8_MAX);

                if (meas_option == 1) {
                    densitometer_t *densitometer = densitometer_transmission();
                    elements.density100 = lroundf(cal_transmission.hi_d * 100);
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate(densitometer, &(cal_transmission.hi_value), sensor_read_callback, &elements);
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
    settings_cal_light_t cal_light;
    settings_cal_gain_t cal_gain;

    settings_get_cal_light(&cal_light);
    settings_get_cal_gain(&cal_gain);

    sprintf_(buf,
        "LR=%d, LT=%d\n"
        "L0 = %.1fx\n"
        "L1 = %.1fx\n"
        "M0 = %.4fx\n"
        "M1 = %.4fx\n"
        "H0 = %.3fx\n"
        "H1 = %.3fx\n"
        "X0 = %.2fx\n"
        "X1 = %.2fx",
        cal_light.reflection, cal_light.transmission,
        1.0F, 1.0F,
        cal_gain.ch0_medium, cal_gain.ch1_medium,
        cal_gain.ch0_high, cal_gain.ch1_high,
        cal_gain.ch0_maximum, cal_gain.ch1_maximum);

    state->cal_sub_option = display_selection_list(
        "Sensor Gain", state->cal_sub_option,
        buf);

    if (state->cal_sub_option == UINT8_MAX) {
        state_controller_set_next_state(controller, STATE_HOME);
    } else if (state->cal_sub_option == 0) {
        state->menu_state = MAIN_MENU_CALIBRATION;
        state->cal_sub_option = 1;
    }
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

void main_menu_settings(state_main_menu_t *state, state_controller_t *controller)
{
    state->settings_option = display_selection_list(
        "Settings", state->settings_option,
        "Target Light\n"
        "USB Key Output\n"
        "Diagnostics");

    if (state->settings_option == 1) {
        state->menu_state = MAIN_MENU_SETTINGS_IDLE_LIGHT;
    } else if (state->settings_option == 2) {
        state->menu_state = MAIN_MENU_SETTINGS_USB_KEY;
    } else if (state->settings_option == 3) {
        state->menu_state = MAIN_MENU_SETTINGS_DIAGNOSTICS;
    } else if (state->settings_option == UINT8_MAX) {
        state_controller_set_next_state(controller, STATE_HOME);
    } else {
        state->menu_state = MAIN_MENU_HOME;
        state->settings_option = 1;
    }
}

void main_menu_settings_idle_light(state_main_menu_t *state, state_controller_t *controller)
{
    char buf[192];

    settings_user_idle_light_t idle_light;
    settings_get_user_idle_light(&idle_light);

    strcpy(buf, "Refl. ");
    if (idle_light.reflection == 0) {
        strcat(buf, "  [None]");
    } else if (idle_light.reflection == SETTING_IDLE_LIGHT_REFL_LOW) {
        strcat(buf, "   [Low]");
    } else if (idle_light.reflection == SETTING_IDLE_LIGHT_REFL_MEDIUM) {
        strcat(buf, "[Medium]");
    } else if (idle_light.reflection == SETTING_IDLE_LIGHT_REFL_HIGH) {
        strcat(buf, "  [High]");
    } else {
        strcat(buf, "     [?]");
    }
    strcat(buf, "\n");

    strcat(buf, "Tran. ");
    if (idle_light.transmission == 0) {
        strcat(buf, "  [None]");
    } else if (idle_light.transmission == SETTING_IDLE_LIGHT_TRAN_LOW) {
        strcat(buf, "   [Low]");
    } else if (idle_light.transmission == SETTING_IDLE_LIGHT_TRAN_MEDIUM) {
        strcat(buf, "[Medium]");
    } else if (idle_light.transmission == SETTING_IDLE_LIGHT_TRAN_HIGH) {
        strcat(buf, "  [High]");
    } else {
        strcat(buf, "     [?]");
    }
    strcat(buf, "\n");

    strcat(buf, "Timeout ");
    if (idle_light.timeout == 0) {
        strcat(buf, "[None]");
    } else if (idle_light.timeout < 10) {
        sprintf(buf + strlen(buf), "  [%ds]", idle_light.timeout);
    } else if (idle_light.timeout < 100) {
        sprintf(buf + strlen(buf), " [%ds]", idle_light.timeout);
    } else {
        sprintf(buf + strlen(buf), "[%ds]", idle_light.timeout);
    }

    state->settings_sub_option = display_selection_list(
        "Target Light", state->settings_sub_option,
        buf);

    if (state->settings_sub_option == 1) {
        if (idle_light.reflection == 0) {
            idle_light.reflection = SETTING_IDLE_LIGHT_REFL_LOW;
        } else if (idle_light.reflection == SETTING_IDLE_LIGHT_REFL_LOW) {
            idle_light.reflection = SETTING_IDLE_LIGHT_REFL_MEDIUM;
        } else if (idle_light.reflection == SETTING_IDLE_LIGHT_REFL_MEDIUM) {
            idle_light.reflection = SETTING_IDLE_LIGHT_REFL_HIGH;
        } else if (idle_light.reflection == SETTING_IDLE_LIGHT_REFL_HIGH) {
            idle_light.reflection = 0;
        } else {
            idle_light.reflection = SETTING_IDLE_LIGHT_REFL_DEFAULT;
        }
        settings_set_user_idle_light(&idle_light);

    } else if (state->settings_sub_option == 2) {
        if (idle_light.transmission == 0) {
            idle_light.transmission = SETTING_IDLE_LIGHT_TRAN_LOW;
        } else if (idle_light.transmission == SETTING_IDLE_LIGHT_TRAN_LOW) {
            idle_light.transmission = SETTING_IDLE_LIGHT_TRAN_MEDIUM;
        } else if (idle_light.transmission == SETTING_IDLE_LIGHT_TRAN_MEDIUM) {
            idle_light.transmission = SETTING_IDLE_LIGHT_TRAN_HIGH;
        } else if (idle_light.transmission == SETTING_IDLE_LIGHT_TRAN_HIGH) {
            idle_light.transmission = 0;
        } else {
            idle_light.transmission = SETTING_IDLE_LIGHT_TRAN_DEFAULT;
        }
        settings_set_user_idle_light(&idle_light);

    } else if (state->settings_sub_option == 3) {
        if (idle_light.timeout < 10) {
            idle_light.timeout = 10;
        } else if (idle_light.timeout < 30) {
            idle_light.timeout = 30;
        } else if (idle_light.timeout < 60) {
            idle_light.timeout = 60;
        } else if (idle_light.timeout < 120) {
            idle_light.timeout = 120;
        } else {
            idle_light.timeout = 0;
        }
        settings_set_user_idle_light(&idle_light);

    } else if (state->settings_sub_option == UINT8_MAX) {
        state_controller_set_next_state(controller, STATE_HOME);
    } else {
        state->menu_state = MAIN_MENU_SETTINGS;
        state->settings_sub_option = 1;
    }
}

void main_menu_settings_usb_key(state_main_menu_t *state, state_controller_t *controller)
{
    char buf[192];

    settings_user_usb_key_t usb_key;
    settings_get_user_usb_key(&usb_key);

    strcpy(buf, "Enabled");
    if (usb_key.enabled) {
        strcat(buf, "  [Yes]");
    } else {
        strcat(buf, "   [No]");
    }
    strcat(buf, "\n");

    strcat(buf, "Fmt.");
    if (usb_key.format == SETTING_KEY_FORMAT_FULL) {
        strcat(buf, " [M+#.##D]");
    } else {
        strcat(buf, "    [#.##]");
    }
    strcat(buf, "\n");

    strcat(buf, "Sep.");
    switch (usb_key.separator) {
    case SETTING_KEY_SEPARATOR_ENTER:
        strcat(buf, "   [Enter]");
        break;
    case SETTING_KEY_SEPARATOR_TAB:
        strcat(buf, "     [Tab]");
        break;
    case SETTING_KEY_SEPARATOR_COMMA:
        strcat(buf, "   [Comma]");
        break;
    case SETTING_KEY_SEPARATOR_SPACE:
        strcat(buf, "   [Space]");
        break;
    case SETTING_KEY_SEPARATOR_NONE:
    default:
        strcat(buf, "    [None]");
        break;
    }

    state->settings_sub_option = display_selection_list(
        "USB Key Output", state->settings_sub_option,
        buf);

    if (state->settings_sub_option == 1) {
        usb_key.enabled = !usb_key.enabled;
        settings_set_user_usb_key(&usb_key);
        usb_device_reconnect();
    } else if (state->settings_sub_option == 2) {
        usb_key.format++;
        if (usb_key.format >= SETTING_KEY_FORMAT_MAX) {
            usb_key.format = 0;
        }
        settings_set_user_usb_key(&usb_key);
    } else if (state->settings_sub_option == 3) {
        usb_key.separator++;
        if (usb_key.separator >= SETTING_KEY_SEPARATOR_MAX) {
            usb_key.separator = 0;
        }
        settings_set_user_usb_key(&usb_key);
    } else if (state->settings_sub_option == UINT8_MAX) {
        state_controller_set_next_state(controller, STATE_HOME);
    } else {
        state->menu_state = MAIN_MENU_SETTINGS;
        state->settings_sub_option = 1;
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
