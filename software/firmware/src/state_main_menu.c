#include "state_main_menu.h"

#define LOG_TAG "state_main_menu"

#include <stdio.h>
#include <math.h>
#include <elog.h>

#include "display.h"
#include "sensor.h"
#include "light.h"
#include "densitometer.h"
#include "settings.h"
#include "util.h"
#include "keypad.h"
#include "tsl2591.h"

extern I2C_HandleTypeDef hi2c1;

typedef enum {
    MAIN_MENU_HOME,
    MAIN_MENU_CALIBRATION,
    MAIN_MENU_CALIBRATION_REFLECTION,
    MAIN_MENU_CALIBRATION_TRANSMISSION,
    MAIN_MENU_CALIBRATION_SENSOR,
    MAIN_MENU_SETTINGS,
    MAIN_MENU_SETTINGS_DIAGNOSTICS,
    MAIN_MENU_ABOUT
} main_menu_state_t;

typedef struct {
    state_t base;
    state_identifier_t last_display_state;
    uint8_t home_option;
    uint8_t cal_option;
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
    .last_display_state = STATE_HOME,
    .home_option = 1,
    .cal_option = 1,
    .settings_option = 1,
    .menu_state = MAIN_MENU_HOME
};

static void main_menu_home(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_reflection(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_transmission(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_sensor(state_main_menu_t *state, state_controller_t *controller);
static void sensor_gain_calibration_callback(sensor_gain_calibration_status_t status, void *user_data);
static void sensor_time_calibration_callback(tsl2591_time_t time, void *user_data);
static void main_menu_settings(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_settings_diagnostics(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_about(state_main_menu_t *state, state_controller_t *controller);
static void sensor_read_callback(void *user_data);

state_t *state_main_menu()
{
    return (state_t *)&state_main_menu_data;
}

void state_main_menu_entry(state_t *state_base, state_controller_t *controller, state_identifier_t prev_state)
{
    state_main_menu_t *state = (state_main_menu_t *)state_base;
    if (prev_state == STATE_REFLECTION_DISPLAY || prev_state == STATE_TRANSMISSION_DISPLAY) {
        state->last_display_state = prev_state;
    }
    state->home_option = 1;
    state->cal_option = 1;
    state->settings_option = 1;
    state->menu_state = MAIN_MENU_HOME;
}

void state_main_menu_process(state_t *state_base, state_controller_t *controller)
{
    state_main_menu_t *state = (state_main_menu_t *)state_base;

    light_set_reflection(0);
    light_set_transmission(0);

    if (state->menu_state == MAIN_MENU_HOME) {
        main_menu_home(state, controller);
    } else if (state->menu_state == MAIN_MENU_CALIBRATION) {
        main_menu_calibration(state, controller);
    } else if (state->menu_state == MAIN_MENU_CALIBRATION_REFLECTION) {
        main_menu_calibration_reflection(state, controller);
    } else if (state->menu_state == MAIN_MENU_CALIBRATION_TRANSMISSION) {
        main_menu_calibration_transmission(state, controller);
    } else if (state->menu_state == MAIN_MENU_CALIBRATION_SENSOR) {
        main_menu_calibration_sensor(state, controller);
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
        state_controller_set_next_state(controller, state->last_display_state);
    }
}

void main_menu_calibration(state_main_menu_t *state, state_controller_t *controller)
{
    state->cal_option = display_selection_list(
        "Calibration", state->cal_option,
        "Reflection\n"
        "Transmission\n"
        "Sensor");

    if (state->cal_option == 1) {
        state->menu_state = MAIN_MENU_CALIBRATION_REFLECTION;
    } else if (state->cal_option == 2) {
        state->menu_state = MAIN_MENU_CALIBRATION_TRANSMISSION;
    } else if (state->cal_option == 3) {
        state->menu_state = MAIN_MENU_CALIBRATION_SENSOR;
    } else if (state->cal_option == UINT8_MAX) {
        state_controller_set_next_state(controller, state->last_display_state);
    } else {
        state->menu_state = MAIN_MENU_HOME;
        state->cal_option = 1;
    }
}

void main_menu_calibration_reflection(state_main_menu_t *state, state_controller_t *controller)
{
    char buf[128];
    char numbuf1[16];
    char numbuf2[16];
    float cal_lo_d;
    float cal_hi_d;
    uint8_t option = 1;

    light_set_reflection(LIGHT_REFLECTION_IDLE);
    light_set_transmission(0);

    settings_get_cal_reflection_lo(&cal_lo_d, NULL);
    settings_get_cal_reflection_hi(&cal_hi_d, NULL);

    do {
        float_to_str(cal_lo_d, numbuf1, 2);
        float_to_str(cal_hi_d, numbuf2, 2);

        sprintf(buf,
            "CAL-LO  [%4s]\n"
            "CAL-HI  [%4s]\n"
            "** Measure **",
            numbuf1, numbuf2);

        option = display_selection_list(
            "Reflection", option, buf);

        if (option == 1) {
            uint16_t working_value = lroundf(cal_lo_d * 100);
            uint8_t input_option = display_input_value_f1_2(
                "CAL-LO (White)\n",
                "D=", &working_value,
                0, 250, NULL);
            if (input_option == 1) {
                cal_lo_d = working_value / 100.0F;
            } else if (input_option == UINT8_MAX) {
                option = UINT8_MAX;
            }

        } else if (option == 2) {
            uint16_t working_value = lroundf(cal_hi_d * 100);
            uint8_t input_option = display_input_value_f1_2(
                "CAL-HI (Black)\n",
                "D=", &working_value,
                0, 250, NULL);
            if (input_option == 1) {
                cal_hi_d = working_value / 100.0F;
            } else if (input_option == UINT8_MAX) {
                option = UINT8_MAX;
            }

        } else if (option == 3) {
            densitometer_result_t meas_result = DENSITOMETER_OK;
            uint8_t meas_option = 0;
            display_main_elements_t elements = {
                .title = "Calibrating...",
                .mode = DISPLAY_MODE_REFLECTION,
                .density100 = 0,
                .frame = 0
            };

            do {
                meas_option = display_message(
                    "Position\n"
                    "CAL-LO firmly\n"
                    "under sensor",
                    NULL, NULL, " Measure ");
                if (meas_option == 1) {
                    elements.density100 = lroundf(cal_lo_d * 100);
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate_reflection_lo(cal_lo_d, sensor_read_callback, &elements);
                } else {
                    break;
                }
                if (meas_result != DENSITOMETER_OK) { break; }

                meas_option = display_message(
                    "Position\n"
                    "CAL-HI firmly\n"
                    "under sensor",
                    NULL, NULL, " Measure ");
                if (meas_option == 1) {
                    elements.density100 = lroundf(cal_hi_d * 100);
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate_reflection_hi(cal_hi_d, sensor_read_callback, &elements);
                } else {
                    break;
                }
                if (meas_result != DENSITOMETER_OK) { break; }
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
            }
        }
    } while (option > 0 && option != UINT8_MAX);

    if (option == UINT8_MAX) {
        state_controller_set_next_state(controller, state->last_display_state);
    } else {
        state->menu_state = MAIN_MENU_CALIBRATION;
    }
}

void main_menu_calibration_transmission(state_main_menu_t *state, state_controller_t *controller)
{
    char buf[128];
    char numbuf[16];
    float cal_hi_d;
    uint8_t option = 1;

    light_set_reflection(0);
    light_set_transmission(LIGHT_TRANSMISSION_IDLE);

    settings_get_cal_transmission_hi(&cal_hi_d, NULL);

    do {
        float_to_str(cal_hi_d, numbuf, 2);

        sprintf(buf,
            "CAL-HI  [%4s]\n"
            "** Measure **",
            numbuf);

        option = display_selection_list(
            "Transmission", option, buf);

        if (option == 1) {
            uint16_t working_value = lroundf(cal_hi_d * 100);
            uint8_t input_option = display_input_value_f1_2(
                "CAL-HI\n",
                "D=", &working_value,
                0, 400, NULL);
            if (input_option == 1) {
                cal_hi_d = working_value / 100.0F;
            } else if (input_option == UINT8_MAX) {
                option = UINT8_MAX;
            }

        } else if (option == 2) {
            densitometer_result_t meas_result = DENSITOMETER_OK;
            uint8_t meas_option = 0;
            display_main_elements_t elements = {
                .title = "Calibrating...",
                .mode = DISPLAY_MODE_TRANSMISSION,
                .density100 = 0,
                .frame = 0
            };

            do {
                meas_option = display_message(
                    "Hold device\n"
                    "firmly closed\n"
                    "with no film",
                    NULL, NULL, " Measure ");
                if (meas_option == 1) {
                    elements.density100 = 0;
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate_transmission_zero(sensor_read_callback, &elements);
                } else {
                    break;
                }
                if (meas_result != DENSITOMETER_OK) { break; }

                meas_option = display_message(
                    "Position\n"
                    "CAL-HI firmly\n"
                    "under sensor",
                    NULL, NULL, " Measure ");
                if (meas_option == 1) {
                    elements.density100 = lroundf(cal_hi_d * 100);
                    elements.frame = 0;
                    display_draw_main_elements(&elements);
                    meas_result = densitometer_calibrate_transmission_hi(cal_hi_d, sensor_read_callback, &elements);
                } else {
                    break;
                }
                if (meas_result != DENSITOMETER_OK) { break; }
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
            }
        }
    } while (option > 0 && option != UINT8_MAX);

    if (option == UINT8_MAX) {
        state_controller_set_next_state(controller, state->last_display_state);
    } else {
        state->menu_state = MAIN_MENU_CALIBRATION;
    }
}

void main_menu_calibration_sensor(state_main_menu_t *state, state_controller_t *controller)
{
    uint8_t option = display_message(
        "Hold device\n"
        "firmly closed\n"
        "with no film", NULL, NULL, " Measure ");
    if (option == 1) {
        HAL_StatusTypeDef ret = HAL_OK;
        do {
            display_static_list("Sensor Gain", "\nStarting...");
            ret = sensor_gain_calibration(sensor_gain_calibration_callback, NULL);

            display_static_list("Int Time", "\nStarting...");
            ret = sensor_time_calibration(sensor_time_calibration_callback, NULL);
        } while (0);

        if (ret == HAL_OK) {
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
        state_controller_set_next_state(controller, state->last_display_state);
    } else {
        state->menu_state = MAIN_MENU_CALIBRATION;
    }
}

void sensor_gain_calibration_callback(sensor_gain_calibration_status_t status, void *user_data)
{
    char buf[128];
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
    } else {
        return;
    }

    display_static_list("Sensor Gain", buf);
}

void sensor_time_calibration_callback(tsl2591_time_t time, void *user_data)
{
    char buf[128];
    uint16_t time_ms = tsl2591_get_time_value_ms(time);
    if (time_ms > 0) {
        sprintf(buf,
            "Measuring\n"
            "%dms\n"
            "integration", time_ms);
        display_static_list("Int Time", buf);
    }
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
    HAL_StatusTypeDef ret = HAL_OK;
    tsl2591_gain_t gain;
    tsl2591_time_t time;
    uint16_t ch0_val = 0;
    uint16_t ch1_val = 0;
    float ch0_basic = NAN;
    float ch1_basic = NAN;
    uint8_t light_mode = 0;
    char gain_ch = ' ';
    char light_ch = ' ';
    bool display_mode = false;
    bool config_changed = true;
    bool settings_changed = true;
    char buf[128];
    char numbuf1[16];
    char numbuf2[16];

    do {
        bool valid = false;

        ret = tsl2591_enable(&hi2c1);
        if (ret != HAL_OK) { break; }

        ret = tsl2591_get_config(&hi2c1, &gain, &time);
        if (ret != HAL_OK) { break; }

        do {
            ret = tsl2591_get_status_valid(&hi2c1, &valid);
        } while (!valid && ret == HAL_OK);
        if (ret != HAL_OK) { break; }
    } while (0);

    if (ret != HAL_OK) {
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
            }
        }

        if (config_changed) {
            ret = tsl2591_set_config(&hi2c1, gain, time);
            if (ret != HAL_OK) { break; }
            config_changed = false;
            settings_changed = true;
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
                light_set_reflection(0);
                light_set_transmission(0);
                light_ch = '-';
                break;
            case 1:
                light_set_reflection(128);
                light_set_transmission(0);
                light_ch = 'R';
                break;
            case 2:
                light_set_reflection(0);
                light_set_transmission(128);
                light_ch = 'T';
                break;
            default:
                light_set_reflection(0);
                light_set_transmission(0);
                light_ch = ' ';
                break;
            }
            settings_changed = false;
        }

        if (tsl2591_get_full_channel_data(&hi2c1, &ch0_val, &ch1_val) == HAL_OK) {
            bool is_detect = keypad_is_detect();
            if (display_mode) {
                sensor_convert_to_basic_counts(gain, time, ch0_val, ch1_val, &ch0_basic, &ch1_basic);
                float_to_str(ch0_basic, numbuf1, 5);
                float_to_str(ch1_basic, numbuf2, 5);
                sprintf(buf,
                    "CH0=%s\n"
                    "CH1=%s\n"
                    "[%c][%d][%c][%c]",
                    numbuf1, numbuf2,
                    gain_ch, tsl2591_get_time_value_ms(time), light_ch,
                    (is_detect ? '*' : ' '));
            } else {
                sprintf(buf,
                    "CH0=%5d\n"
                    "CH1=%5d\n"
                    "[%c][%d][%c][%c]",
                    ch0_val, ch1_val,
                    gain_ch, tsl2591_get_time_value_ms(time), light_ch,
                    (is_detect ? '*' : ' '));
            }
            display_static_list("Diagnostics", buf);
        }

        if (keypad_wait_for_event(&keypad_event, 100) == osOK) {
            key_changed = true;
        }
    } while (1);

    state->menu_state = MAIN_MENU_SETTINGS;
}

void main_menu_about(state_main_menu_t *state, state_controller_t *controller)
{
    char buf[128];

    sprintf(buf,
        "Printalyzer\n"
        "Densitometer\n"
        "v0.1");

    uint8_t option = display_message(buf, NULL, NULL, " OK ");
    if (option == UINT8_MAX) {
        state_controller_set_next_state(controller, state->last_display_state);
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
