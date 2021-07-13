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

typedef enum {
    MAIN_MENU_HOME,
    MAIN_MENU_CALIBRATION,
    MAIN_MENU_CALIBRATION_REFLECTION,
    MAIN_MENU_CALIBRATION_TRANSMISSION,
    MAIN_MENU_CALIBRATION_SENSOR_GAIN,
    MAIN_MENU_SETTINGS,
    MAIN_MENU_ABOUT
} main_menu_state_t;

typedef struct {
    state_t base;
    state_identifier_t last_display_state;
    uint8_t home_option;
    uint8_t cal_option;
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
    .menu_state = MAIN_MENU_HOME
};

static void main_menu_home(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_reflection(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_transmission(state_main_menu_t *state, state_controller_t *controller);
static void main_menu_calibration_sensor_gain(state_main_menu_t *state, state_controller_t *controller);
static void sensor_gain_calibration_callback(sensor_gain_calibration_status_t status, void *user_data);
static void main_menu_settings(state_main_menu_t *state, state_controller_t *controller);
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
    } else if (state->menu_state == MAIN_MENU_CALIBRATION_SENSOR_GAIN) {
        main_menu_calibration_sensor_gain(state, controller);
    } else if (state->menu_state == MAIN_MENU_SETTINGS) {
        main_menu_settings(state, controller);
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
        "Sensor Gain");

    if (state->cal_option == 1) {
        state->menu_state = MAIN_MENU_CALIBRATION_REFLECTION;
    } else if (state->cal_option == 2) {
        state->menu_state = MAIN_MENU_CALIBRATION_TRANSMISSION;
    } else if (state->cal_option == 3) {
        state->menu_state = MAIN_MENU_CALIBRATION_SENSOR_GAIN;
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

void main_menu_calibration_sensor_gain(state_main_menu_t *state, state_controller_t *controller)
{
    uint8_t option = display_message(
        "Hold device\n"
        "firmly closed\n"
        "with no film", NULL, NULL, " Measure ");
    if (option == 1) {
        HAL_StatusTypeDef ret = sensor_gain_calibration(sensor_gain_calibration_callback, NULL);

        if (ret == HAL_OK) {
            display_message(
                "Sensor Gain", NULL,
                "calibration\n"
                "complete", " OK ");
        } else {
            display_message(
                "Sensor Gain", NULL,
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
    if (status == SENSOR_GAIN_CALIBRATION_STATUS_INIT) {
        sprintf(buf, "\nStarting...");
    } else if (status == SENSOR_GAIN_CALIBRATION_STATUS_MEDIUM) {
        sprintf(buf,"\n"
            "Measuring\n"
            "medium gain\n...");
    } else if (status == SENSOR_GAIN_CALIBRATION_STATUS_HIGH) {
        sprintf(buf,"\n"
            "Measuring\n"
            "high gain\n...");
    } else if (status == SENSOR_GAIN_CALIBRATION_STATUS_MAXIMUM) {
        sprintf(buf,"\n"
            "Measuring\n"
            "max gain\n...");
    } else {
        return;
    }

    display_static_list("Sensor Gain", buf);
}

void main_menu_settings(state_main_menu_t *state, state_controller_t *controller)
{
    //TODO
    state->menu_state = MAIN_MENU_HOME;
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
