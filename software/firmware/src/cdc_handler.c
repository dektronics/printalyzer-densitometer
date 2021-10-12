#include "cdc_handler.h"

#define LOG_TAG "cdc_handler"
#include <elog.h>
#include <elog_port.h>

#include "stm32l0xx_hal.h"

#include <printf.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <machine/endian.h>
#include <math.h>
#include <tusb.h>
#include <cmsis_os.h>
#include <FreeRTOS.h>

#include "settings.h"
#include "display.h"
#include "light.h"
#include "sensor.h"
#include "task_main.h"
#include "task_sensor.h"
#include "adc_handler.h"
#include "tsl2591.h"
#include "densitometer.h"
#include "app_descriptor.h"
#include "util.h"

#define CMD_DATA_SIZE 64
#define CDC_TX_TIMEOUT 200
#define CDC_MIN_BIT_RATE 9600

typedef enum {
    CMD_TYPE_SET,
    CMD_TYPE_GET,
    CMD_TYPE_INVOKE
} cmd_type_t;

typedef enum {
    CMD_CATEGORY_SYSTEM,
    CMD_CATEGORY_MEASUREMENT,
    CMD_CATEGORY_CALIBRATION,
    CMD_CATEGORY_DIAGNOSTICS
} cmd_category_t;

typedef struct {
    cmd_type_t type;
    cmd_category_t category;
    char action[8];
    char args[32];
} cdc_command_t;

static volatile bool cdc_initialized = false;
static volatile bool cdc_host_connected = false;
static volatile bool cdc_logging_redirected = false;
static uint8_t cmd_buffer[CMD_DATA_SIZE];
static size_t cmd_buffer_len = 0;
static bool cdc_remote_enabled = false;
static volatile bool cdc_remote_active = false;

/* Semaphore used to unblock the task when new data is available */
static osSemaphoreId_t cdc_rx_semaphore = NULL;
static const osSemaphoreAttr_t cdc_rx_semaphore_attrs = {
    .name = "cdc_rx_semaphore"
};

/* Semaphore used to synchronize data writing */
static osSemaphoreId_t cdc_tx_semaphore = NULL;
static const osSemaphoreAttr_t cdc_tx_semaphore_attrs = {
    .name = "cdc_tx_semaphore"
};

/* Mutex used to allow CDC writes from different tasks */
static osMutexId_t cdc_mutex = NULL;
static const osMutexAttr_t cdc_mutex_attrs = {
    .name = "cdc_mutex"
};

static void cdc_task_loop();
static void cdc_set_connected(bool connected);
static void cdc_process_command(const char *buf, size_t len);
static bool cdc_parse_command(cdc_command_t *cmd, const char *buf, size_t len);
static bool cdc_process_command_system(const cdc_command_t *cmd);
static bool cdc_process_command_measurement(const cdc_command_t *cmd);
static bool cdc_process_command_calibration(const cdc_command_t *cmd);
static bool cdc_process_command_diagnostics(const cdc_command_t *cmd);

static void cdc_send_response(const char *str);
static void cdc_send_command_response(const cdc_command_t *cmd, const char *str);

extern I2C_HandleTypeDef hi2c1;

void task_cdc_run(void *argument)
{
    osSemaphoreId_t task_start_semaphore = argument;

    log_d("cdc_task start");

    /* Create the CDC RX semaphore */
    cdc_rx_semaphore = osSemaphoreNew(1, 0, &cdc_rx_semaphore_attrs);
    if (!cdc_rx_semaphore) {
        log_e("cdc_rx_semaphore create error");
        return;
    }

    /* Create the CDC TX semaphore */
    cdc_tx_semaphore = osSemaphoreNew(1, 0, &cdc_tx_semaphore_attrs);
    if (!cdc_tx_semaphore) {
        log_e("cdc_tx_semaphore create error");
        return;
    }

    /* Create the CDC write mutex */
    cdc_mutex = osMutexNew(&cdc_mutex_attrs);
    if (!cdc_mutex) {
        log_e("Unable to create cdc_mutex");
        return;
    }

    cdc_initialized = true;

    /* Release the startup semaphore */
    if (osSemaphoreRelease(task_start_semaphore) != osOK) {
        log_e("Unable to release task_start_semaphore");
        return;
    }

    while (1) {
        /* Process data */
        cdc_task_loop();

        /* Block for new data */
        if (osSemaphoreAcquire(cdc_rx_semaphore, portMAX_DELAY) != osOK) {
            log_e("Unable to acquire cdc_rx_semaphore");
        }
    }
}

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    if (!cdc_logging_redirected) {
        log_d("tud_cdc_line_state: itf=%d, dtr=%d, rts=%d", itf, dtr, rts);
    }
    osMutexAcquire(cdc_mutex, portMAX_DELAY);
    if (dtr) {
        cdc_line_coding_t *coding = NULL;
        tud_cdc_get_line_coding(coding);
        cdc_set_connected(!coding || (coding->bit_rate >= CDC_MIN_BIT_RATE));
    } else {
        cdc_set_connected(false);
    }
    osMutexRelease(cdc_mutex);
}

void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding)
{
    if (!cdc_logging_redirected) {
        log_d("tud_cdc_line_coding: itf=%d, bit_rate=%d", itf, p_line_coding->bit_rate);
    }
    osMutexAcquire(cdc_mutex, portMAX_DELAY);
    if (p_line_coding->bit_rate < CDC_MIN_BIT_RATE) {
        log_w("Bit rate not supported: %d < %d", p_line_coding->bit_rate, CDC_MIN_BIT_RATE);
        cdc_set_connected(false);
    }
    osMutexRelease(cdc_mutex);
}

void tud_cdc_tx_complete_cb(uint8_t itf)
{
    /* log_d("tud_cdc_tx_complete_cb: itf=%d", itf); */
    if (!cdc_initialized) { return; }
    osSemaphoreRelease(cdc_tx_semaphore);
}

void tud_cdc_rx_cb(uint8_t itf)
{
    /* log_d("tud_cdc_rx_cb: itf=%d", itf); */
    if (!cdc_initialized) { return; }
    osSemaphoreRelease(cdc_rx_semaphore);
}

void cdc_task_loop()
{
    /*
     * Notes from example:
     * connected() check for DTR bit
     * Most but not all terminal client set this when making connection
     * if ( tud_cdc_connected() )
     */

    if (tud_cdc_available()) {
        /* Read data into local buffer */
        char buf[64];
        uint32_t count = tud_cdc_read(buf, sizeof(buf));

        for (size_t i = 0; i < count; i++) {
            /* Fill buffer as long as there is space */
            if (cmd_buffer_len < CMD_DATA_SIZE) {
                if (buf[i] >= 0x20 && buf[i] < 0x7F) {
                    /* Only fill buffer with printable characters */
                    cmd_buffer[cmd_buffer_len++] = buf[i];
                } else if (buf[i] == 0x08 || buf[i] == 0x7F) {
                    /* Handle backspace behavior */
                    if (cmd_buffer_len > 0) {
                        cmd_buffer_len--;
                        cmd_buffer[cmd_buffer_len] = '\0';
                    }
                }
            }
            /* Accept command as soon as a line break is sent */
            if ((buf[i] == '\r' || buf[i] == '\n') && cmd_buffer_len > 0) {
                /* Accept command */
                cmd_buffer[cmd_buffer_len] = '\0';
                cdc_process_command((const char *)cmd_buffer, cmd_buffer_len);

                /* Clear command buffer */
                memset(cmd_buffer, 0, sizeof(cmd_buffer));
                cmd_buffer_len = 0;
            }
        }
    }
}

void cdc_set_connected(bool connected)
{
    if (cdc_host_connected != connected) {
        if (!connected) {
            elog_port_redirect(NULL);
            elog_set_text_color_enabled(true);
            cdc_logging_redirected = false;
            if (cdc_remote_enabled) {
                task_main_force_state(STATE_HOME);
                cdc_remote_enabled = false;
                cdc_remote_active = false;
            }
        }
        cdc_host_connected = connected;
    }
}

void cdc_process_command(const char *buf, size_t len)
{
    cdc_command_t cmd = {0};

    if (len == 0 || buf[0] == '\0') {
        return;
    }

    if (cdc_parse_command(&cmd, buf, len)) {
        bool result = false;
        switch (cmd.category) {
        case CMD_CATEGORY_SYSTEM:
            result = cdc_process_command_system(&cmd);
            break;
        case CMD_CATEGORY_MEASUREMENT:
            result = cdc_process_command_measurement(&cmd);
            break;
        case CMD_CATEGORY_CALIBRATION:
            result = cdc_process_command_calibration(&cmd);
            break;
        case CMD_CATEGORY_DIAGNOSTICS:
            result = cdc_process_command_diagnostics(&cmd);
            break;
        default:
            break;
        }
        if (!result) {
            cdc_send_command_response(&cmd, "NAK");
        }
    }
}

bool cdc_parse_command(cdc_command_t *cmd, const char *buf, size_t len)
{
    cmd_type_t type;
    cmd_category_t category;

    if (!cmd || !buf || len < 2 || buf[0] == '\0') {
        return false;
    }

    switch (buf[0]) {
    case 'S':
        type = CMD_TYPE_SET;
        break;
    case 'G':
        type = CMD_TYPE_GET;
        break;
    case 'I':
        type = CMD_TYPE_INVOKE;
        break;
    default:
        return false;
    }

    switch (buf[1]) {
    case 'S':
        category = CMD_CATEGORY_SYSTEM;
        break;
    case 'M':
        category = CMD_CATEGORY_MEASUREMENT;
        break;
    case 'C':
        category = CMD_CATEGORY_CALIBRATION;
        break;
    case 'D':
        category = CMD_CATEGORY_DIAGNOSTICS;
        break;
    default:
        return false;
    }

    if (len > 2 && buf[2] != ' ') {
        return false;
    }

    cmd->type = type;
    cmd->category = category;

    if (len > 3) {
        char *p = strchr(buf + 3, ',');
        if (p) {
            strncpy(cmd->action, buf + 3, MIN(p - (buf + 3), sizeof(cmd->action) - 1));
            strncpy(cmd->args, p + 1, MIN(len - ((p + 1) - buf), sizeof(cmd->args) - 1));
        } else {
            strncpy(cmd->action, buf + 3, MIN(len - 3, sizeof(cmd->action) - 1));
            bzero(cmd->args, sizeof(cmd->args));
        }
    } else {
        bzero(cmd->action, sizeof(cmd->action));
        bzero(cmd->args, sizeof(cmd->args));
    }

    log_i("Command: [%c][%c] {%s},\"%s\"", buf[0], buf[1], cmd->action, cmd->args);

    return true;
}

bool cdc_process_command_system(const cdc_command_t *cmd)
{
    /*
     * System Commands
     * "GS V"    -> Get project name and version
     * "GS B"    -> Get firmware build information
     * "GS DEV"  -> Get device information (HAL version, MCU Rev ID, MCU Dev ID, SysClock)
     * "GS RTOS" -> Get FreeRTOS information
     * "GS UID"  -> Get device unique ID
     * "GS ISEN" -> Internal sensor readings
     * "IS REMOTE,n" -> Invoke remote control mode (enable = 1, disable = 0)
     */
    const app_descriptor_t *app_descriptor = app_descriptor_get();
    char buf[128];

    if (!cmd) { return false; }

    if (cmd->type == CMD_TYPE_GET && strcmp(cmd->action, "V") == 0) {
        /*
         * Output format:
         * Project name, Version
         */
        sprintf(buf, "\"%s\",\"%s\"", app_descriptor->project_name, app_descriptor->version);
        cdc_send_command_response(cmd, buf);
        return true;
    } else if (cmd->type == CMD_TYPE_GET && strcmp(cmd->action, "B") == 0) {
        /*
         * Output format:
         * Build date, Build describe, Checksum
         */
        sprintf(buf, "\"%s\",\"%s\",%08lX",
            app_descriptor->build_date,
            app_descriptor->build_describe,
            __bswap32(app_descriptor->crc32));
        cdc_send_command_response(cmd, buf);
        return true;
    } else if (cmd->type == CMD_TYPE_GET && strcmp(cmd->action, "DEV") == 0) {
        /*
         * Output format:
         * HAL Version, MCU Device ID, MCU Revision ID, SysClock Frequency
         */
        uint32_t hal_ver = HAL_GetHalVersion();
        uint8_t hal_ver_code = ((uint8_t)(hal_ver)) & 0x0F;

        sprintf(buf, "%d.%d.%d%c,0x%lX,0x%lX,%ldMHz",
            ((uint8_t)(hal_ver >> 24)) & 0x0F,
            ((uint8_t)(hal_ver >> 16)) & 0x0F,
            ((uint8_t)(hal_ver >> 8)) & 0x0F,
            (hal_ver_code > 0 ? (char)hal_ver_code : ' '),
            HAL_GetDEVID(),
            HAL_GetREVID(),
            HAL_RCC_GetSysClockFreq() / 1000000);
        cdc_send_command_response(cmd, buf);
        return true;
    } else if (cmd->type == CMD_TYPE_GET && strcmp(cmd->action, "RTOS") == 0) {
        /*
         * Output format:
         * FreeRTOS Version, Heap Size, Heap Watermark, Task Count
         */
        sprintf(buf, "%s,%d,%d,%ld",
            tskKERNEL_VERSION_NUMBER,
            xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize(),
            uxTaskGetNumberOfTasks());
        cdc_send_command_response(cmd, buf);
        return true;
    } else if (cmd->type == CMD_TYPE_GET && strcmp(cmd->action, "UID") == 0) {
        uint8_t *uniqueId = (uint8_t*)UID_BASE;

        sprintf(buf, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
            uniqueId[0], uniqueId[1], uniqueId[2], uniqueId[3], uniqueId[4],
            uniqueId[5], uniqueId[6], uniqueId[7], uniqueId[8], uniqueId[9],
            uniqueId[10], uniqueId[11]);
        cdc_send_command_response(cmd, buf);
        return true;
    } else if (cmd->type == CMD_TYPE_GET && strcmp(cmd->action, "ISEN") == 0) {
        /*
         * Output format:
         * VDDA, Temperature
         */
        adc_readings_t readings;
        if (adc_read(&readings) == osOK) {
            sprintf(buf, "%dmV,%dC", readings.vdda_mv, readings.temp_c);
            cdc_send_command_response(cmd, buf);
        } else {
            cdc_send_command_response(cmd, "ERR");
        }
        return true;
    } else if (cmd->type == CMD_TYPE_INVOKE && strcmp(cmd->action, "REMOTE") == 0) {
        bool enable;
        if (cmd->args[0] == '0' && cmd->args[1] == '\0') {
            enable = false;
        } else if (cmd->args[0] == '1' && cmd->args[1] == '\0') {
            enable = true;
        } else {
            return false;
        }

        log_i("Set remote control mode: %d", enable);
        if (enable) {
            cdc_remote_enabled = true;
            task_main_force_state(STATE_REMOTE);
        } else {
            task_main_force_state(STATE_HOME);
            cdc_remote_enabled = false;
        }

        return true;
    } else {
        return false;
    }
}

bool cdc_process_command_measurement(const cdc_command_t *cmd)
{
    /*
     * It might make more sense to simply remove the explicit measurement
     * commands, because they no longer make any sense here.
     * In their place, perhaps this is where we should put commands to
     * change the measurement response format from the default (just density)
     * to an expanded version with raw/intermediate sensor readings that
     * are useful to offline calibration routines.
     */
    //TODO Add commands to change the measurement response format
    return false;
}

bool cdc_process_command_calibration(const cdc_command_t *cmd)
{
    /*
     * Calibration Commands
     * "IC GAIN" -> Invoke the sensor gain calibration process [remote]
     * "IC LR"   -> Invoke the reflection light source calibration process [remote]
     * "IC LT"   -> Invoke the transmission light source calibration process [remote]
     *
     * "GC GAIN" -> Get sensor gain calibration values
     * "GC LR"   -> Get reflection light source calibration value
     * "GC LT"   -> Get reflection light source calibration value
     */
    if (cmd->type == CMD_TYPE_INVOKE && strcmp(cmd->action, "GAIN") == 0 && cdc_remote_active) {
        //TODO This is long running, having some sort of progress notification could be helpful
        osStatus_t result = sensor_gain_calibration(NULL, NULL);
        if (result == osOK) {
            cdc_send_command_response(cmd, "OK");
        } else {
            cdc_send_command_response(cmd, "ERR");
        }
        return true;
    } else if (cmd->type == CMD_TYPE_INVOKE && strcmp(cmd->action, "LR") == 0 && cdc_remote_active) {
        //TODO This is long running, having some sort of progress notification could be helpful
        osStatus_t result = sensor_light_calibration(SENSOR_LIGHT_REFLECTION, NULL, NULL);
        if (result == osOK) {
            cdc_send_command_response(cmd, "OK");
        } else {
            cdc_send_command_response(cmd, "ERR");
        }
        return true;
    } else if (cmd->type == CMD_TYPE_INVOKE && strcmp(cmd->action, "LT") == 0 && cdc_remote_active) {
        //TODO This is long running, having some sort of progress notification could be helpful
        osStatus_t result = sensor_light_calibration(SENSOR_LIGHT_TRANSMISSION, NULL, NULL);
        if (result == osOK) {
            cdc_send_command_response(cmd, "OK");
        } else {
            cdc_send_command_response(cmd, "ERR");
        }
        return true;
    } else if (cmd->type == CMD_TYPE_GET && strcmp(cmd->action, "GAIN") == 0) {
        char buf[128];
        float ch0_medium;
        float ch1_medium;
        float ch0_high;
        float ch1_high;
        float ch0_maximum;
        float ch1_maximum;

        settings_get_cal_gain(TSL2591_GAIN_MEDIUM, &ch0_medium, &ch1_medium);
        settings_get_cal_gain(TSL2591_GAIN_HIGH, &ch0_high, &ch1_high);
        settings_get_cal_gain(TSL2591_GAIN_MAXIMUM, &ch0_maximum, &ch1_maximum);

        sprintf_(buf, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
            1.0F, 1.0F,
            ch0_medium, ch1_medium,
            ch0_high, ch1_high,
            ch0_maximum, ch1_maximum);
        cdc_send_command_response(cmd, buf);
        return true;
    } else if (cmd->type == CMD_TYPE_GET && strcmp(cmd->action, "LR") == 0) {
        char buf[64];
        float value;
        settings_get_cal_reflection_led_factor(&value);
        sprintf_(buf, "%f", value);
        cdc_send_command_response(cmd, buf);
        return true;
    } else if (cmd->type == CMD_TYPE_GET && strcmp(cmd->action, "LT") == 0) {
        char buf[64];
        float value;
        settings_get_cal_transmission_led_factor(&value);
        sprintf_(buf, "%f", value);
        cdc_send_command_response(cmd, buf);
        return true;
    }

    return false;
}

bool cdc_process_command_diagnostics(const cdc_command_t *cmd)
{
    /*
     * Diagnostics Commands
     * "GD DISP" -> Get display screenshot (multi-line response)
     *
     * "SD LR,nnn" -> Set reflection light duty cycle (nnn/127) [remote]
     * "SD LT,nnn" -> Set transmission light duty cycle (nnn/127) [remote]
     *
     * "ID S,START"   -> Invoke sensor start [remote]
     * "ID S,STOP"    -> Invoke sensor stop [remote]
     * "SD S,CFG,n,m" -> Set sensor gain (n = [0-3]) and integration time (m = [0-5]) [remote]
     * "GD S,READING" -> Get next sensor reading [remote]
     *
     * "SD LOG,U" -> Set logging output to USB CDC device
     * "SD LOG,D" -> Set logging output to debug port UART
     */

    if (!cmd) { return false; }

    if (cmd->type == CMD_TYPE_GET && strcmp(cmd->action, "DISP") == 0) {
        cdc_send_command_response(cmd, "[[");
        display_capture_screenshot();
        cdc_send_response("]]\r\n");
        return true;
    } else if (cmd->type == CMD_TYPE_SET && strcmp(cmd->action, "LR") == 0 && cdc_remote_active) {
        uint8_t value = atoi(cmd->args);
        if (value > 128) { value = 128; }

        osStatus_t result = sensor_set_light_mode(SENSOR_LIGHT_REFLECTION, false, value);
        if (result == osOK) {
            cdc_send_command_response(cmd, "OK");
        } else {
            cdc_send_command_response(cmd, "ERR");
        }

        return true;
    } else if (cmd->type == CMD_TYPE_SET && strcmp(cmd->action, "LT") == 0 && cdc_remote_active) {
        uint8_t value = atoi(cmd->args);
        if (value > 128) { value = 128; }

        osStatus_t result = sensor_set_light_mode(SENSOR_LIGHT_TRANSMISSION, false, value);
        if (result == osOK) {
            cdc_send_command_response(cmd, "OK");
        } else {
            cdc_send_command_response(cmd, "ERR");
        }

        return true;
    } else if (strcmp(cmd->action, "S") == 0 && cdc_remote_active) {
        osStatus_t result;
        if (cmd->type == CMD_TYPE_INVOKE && strcmp(cmd->args, "START") == 0) {
            result = sensor_start();
            if (result == osOK) {
                cdc_send_command_response(cmd, "OK");
            } else {
                cdc_send_command_response(cmd, "ERR");
            }
            return true;
        } else if (cmd->type == CMD_TYPE_INVOKE && strcmp(cmd->args, "STOP") == 0) {
            result = sensor_stop();
            if (result == osOK) {
                cdc_send_command_response(cmd, "OK");
            } else {
                cdc_send_command_response(cmd, "ERR");
            }
            return true;
        } else if (cmd->type == CMD_TYPE_SET && strncmp(cmd->args, "CFG,", 4) == 0) {
            if (isdigit((unsigned char)cmd->args[4]) && cmd->args[5] == ','
                && isdigit((unsigned char)cmd->args[6]) && cmd->args[7] == '\0') {
                tsl2591_gain_t gain_val = cmd->args[4] - '0';
                tsl2591_time_t time_val = cmd->args[6] - '0';
                if (gain_val <= TSL2591_GAIN_MAXIMUM && time_val <= TSL2591_TIME_600MS) {
                    result = sensor_set_config(gain_val, time_val);
                    if (result == osOK) {
                        cdc_send_command_response(cmd, "OK");
                    } else {
                        cdc_send_command_response(cmd, "ERR");
                    }
                    return true;
                }
            }
        } else if (cmd->type == CMD_TYPE_GET && strcmp(cmd->args, "READING") == 0) {
            /*
             * Output format:
             * CH0 value, CH1 value, Gain setting, Integration time setting
             */
            sensor_reading_t reading;
            result = sensor_get_next_reading(&reading, 1000);
            if (result == osOK) {
                char buf[64];
                sprintf(buf, "%d,%d,%d,%d",
                    reading.ch0_val, reading.ch1_val, reading.gain, reading.time);
                cdc_send_command_response(cmd, buf);
            } else {
                cdc_send_command_response(cmd, "ERR");
            }
            return true;
        }

        return true;
    } else if (cmd->type == CMD_TYPE_SET && strcmp(cmd->action, "LOG") == 0) {
        if (strcmp(cmd->args, "U") == 0) {
            cdc_send_command_response(cmd, "OK");
            cdc_logging_redirected = true;
            elog_set_text_color_enabled(false);
            elog_port_redirect(cdc_write);
            return true;
        } else if (strcmp(cmd->args, "D") == 0) {
            cdc_send_command_response(cmd, "OK");
            elog_port_redirect(NULL);
            elog_set_text_color_enabled(true);
            cdc_logging_redirected = false;
            return true;
        }
    }

    return false;
}

void cdc_send_response(const char *str)
{
    size_t len = strlen(str);
    cdc_write(str, len);
}

void cdc_send_command_response(const cdc_command_t *cmd, const char *str)
{
    char buf[128];
    char t_ch;
    char c_ch;
    if (!cmd || !str) { return; }

    switch (cmd->type) {
    case CMD_TYPE_SET:
        t_ch = 'S';
        break;
    case CMD_TYPE_GET:
        t_ch = 'G';
        break;
    case CMD_TYPE_INVOKE:
        t_ch = 'I';
        break;
    default:
        return;
    }

    switch (cmd->category) {
    case CMD_CATEGORY_SYSTEM:
        c_ch = 'S';
        break;
    case CMD_CATEGORY_MEASUREMENT:
        c_ch = 'M';
        break;
    case CMD_CATEGORY_CALIBRATION:
        c_ch = 'C';
        break;
    case CMD_CATEGORY_DIAGNOSTICS:
        c_ch = 'D';
        break;
    default:
        return;
    }

    size_t n = snprintf(buf, sizeof(buf), "%c%c %s,%s\r\n", t_ch, c_ch, cmd->action, str);
    cdc_write(buf, n);
}

void cdc_send_density_reading(char prefix, float value)
{
    char buf[16];
    char sign;

    /* Force any invalid values to be zero */
    if (isnanf(value) || isinff(value)) {
        value = 0.0F;
    }

    /* Find the sign character */
    if (value >= 0.0F) {
        sign = '+';
    } else {
        sign = '-';
    }

    /* Format the result */
    size_t n = sprintf_(buf, "%c%c%.2fD\r\n", prefix, sign, fabsf(value));

    /* Catch cases where a negative was rounded to zero */
    if (strncmp(buf + 1, "-0.00", 5) == 0) {
        buf[1] = '+';
    }

    cdc_write(buf, n);
}

void cdc_send_remote_state(bool enabled)
{
    osMutexAcquire(cdc_mutex, portMAX_DELAY);
    cdc_remote_active = enabled && cdc_remote_enabled;
    osMutexRelease(cdc_mutex);
    cdc_command_t cmd = {
        .type = CMD_TYPE_INVOKE,
        .category = CMD_CATEGORY_SYSTEM,
        .action = "REMOTE"
    };
    cdc_send_command_response(&cmd, enabled ? "1" : "0");
}

void cdc_write(const char *buf, size_t len)
{
    osMutexAcquire(cdc_mutex, portMAX_DELAY);
    if (cdc_host_connected && len > 0) {
        uint32_t n = 0;
        uint32_t offset = 0;
        do {
            n = tud_cdc_write(buf + offset, len - offset);
            if (n == 0) {
                if (!cdc_logging_redirected) {
                    log_w("Write error");
                }
                break;
            }
            tud_cdc_write_flush();
            offset += n;

            if (osSemaphoreAcquire(cdc_tx_semaphore, CDC_TX_TIMEOUT) != osOK) {
                if (!cdc_logging_redirected) {
                    log_e("Unable to acquire cdc_tx_semaphore");
                }
                tud_cdc_write_clear();
                tud_cdc_abort_transfer();
                cdc_set_connected(false);
                break;
            }
        } while (offset < len);
    }
    osMutexRelease(cdc_mutex);
}
