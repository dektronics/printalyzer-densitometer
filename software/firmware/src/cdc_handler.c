#include "cdc_handler.h"

#define LOG_TAG "cdc_handler"
#include <elog.h>
#include <elog_port.h>

#include "stm32l0xx_hal.h"

#include <printf.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <tusb.h>
#include <cmsis_os.h>
#include <FreeRTOS.h>

#include "settings.h"
#include "display.h"
#include "light.h"
#include "sensor.h"
#include "task_sensor.h"
#include "adc_handler.h"
#include "tsl2591.h"
#include "densitometer.h"
#include "util.h"

#define CMD_DATA_SIZE 64
#define CDC_TX_TIMEOUT 200
#define CDC_MIN_BIT_RATE 9600

static volatile bool cdc_initialized = false;
static volatile bool cdc_host_connected = false;
static uint8_t cmd_buffer[CMD_DATA_SIZE];
static size_t cmd_buffer_len = 0;

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
static void cdc_process_command(const char *cmd, size_t len);
static void cdc_command_version(const char *cmd, size_t len);
static void cdc_command_measure_reflection(const char *cmd, size_t len);
static void cdc_command_measure_transmission(const char *cmd, size_t len);
static void cdc_command_cal_gain(const char *cmd, size_t len);
static void cdc_command_cal_light(const char *cmd, size_t len);
static void cdc_command_cal_reflection(const char *cmd, size_t len);
static void cdc_command_cal_transmission(const char *cmd, size_t len);
static void cdc_command_diag_system(const char *cmd, size_t len);
static void cdc_command_diag_display(const char *cmd, size_t len);
static void cdc_command_diag_light(const char *cmd, size_t len);
static void cdc_command_diag_sensor(const char *cmd, size_t len);
static void cdc_send_response(const char *str);

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
    log_d("tud_cdc_line_state: itf=%d, dtr=%d, rts=%d", itf, dtr, rts);
    osMutexAcquire(cdc_mutex, portMAX_DELAY);
    if (dtr) {
        cdc_line_coding_t *coding = NULL;
        tud_cdc_get_line_coding(coding);
        cdc_host_connected = !coding || (coding->bit_rate >= CDC_MIN_BIT_RATE);
    } else {
        cdc_host_connected = false;
    }
    osMutexRelease(cdc_mutex);
}

void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding)
{
    log_d("tud_cdc_line_coding: itf=%d, bit_rate=%d", itf, p_line_coding->bit_rate);
    osMutexAcquire(cdc_mutex, portMAX_DELAY);
    if (p_line_coding->bit_rate < CDC_MIN_BIT_RATE) {
        log_w("Bit rate not supported: %d < %d", p_line_coding->bit_rate, CDC_MIN_BIT_RATE);
        cdc_host_connected = false;
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

        /* Echo back out the device */
        cdc_write(buf, count);

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

void cdc_process_command(const char *cmd, size_t len)
{
    if (len == 0 || cmd[0] == '\0') {
        return;
    }

    log_i("Command: \"%s\"", cmd);

    char cmd_prefix = toupper(cmd[0]);

    if (cmd_prefix == 'V' && len == 1) {
        /* Version string command */
        cdc_command_version(cmd, len);
    } else if (cmd_prefix == 'M' && len > 1) {
        /* Measurement command */
        char meas_prefix = toupper(cmd[1]);
        if (meas_prefix == 'R') {
            cdc_command_measure_reflection(cmd, len);
        } else if (meas_prefix == 'T') {
            cdc_command_measure_transmission(cmd, len);
        }
    } else if (cmd_prefix == 'C' && len > 1) {
        /* Calibration command */
        char cal_prefix = toupper(cmd[1]);
        if (cal_prefix == 'G') {
            cdc_command_cal_gain(cmd, len);
        } else if (cal_prefix == 'L') {
            cdc_command_cal_light(cmd, len);
        } else if (cal_prefix == 'R') {
            cdc_command_cal_reflection(cmd, len);
        } else if (cal_prefix == 'T') {
            cdc_command_cal_transmission(cmd, len);
        }
    } else if (cmd_prefix == 'D' && len > 1) {
        /* Diagnostic command */
        char diag_prefix = toupper(cmd[1]);
        if (diag_prefix == 'A') {
            cdc_command_diag_system(cmd, len);
        } else if (diag_prefix == 'D') {
            cdc_command_diag_display(cmd, len);
        } else if (diag_prefix == 'L') {
            cdc_command_diag_light(cmd, len);
        } else if (diag_prefix == 'S') {
            cdc_command_diag_sensor(cmd, len);
        }
    } else if (cmd_prefix == 'L' && len > 1) {
        /* Logging command */
        char log_prefix = toupper(cmd[1]);
        if (log_prefix == 'U') {
            elog_port_redirect(cdc_write);
        } else if (log_prefix == 'D') {
            elog_port_redirect(NULL);
        }
    }
}

void cdc_command_version(const char *cmd, size_t len)
{
    UNUSED(cmd);
    UNUSED(len);
    char buf[64];
    sprintf(buf, "Printalyzer Densitometer\r\n");
    cdc_send_response(buf);
}

void cdc_command_measure_reflection(const char *cmd, size_t len)
{
    /*
     * "MR" : Measurement Reflection
     */

    UNUSED(cmd);
    UNUSED(len);

    densitometer_result_t result = densitometer_reflection_measure(NULL, NULL);
    if (result == DENSITOMETER_OK) {
        char buf[128];

        float meas_d = densitometer_reflection_get_last_reading();
        sprintf_(buf, "MR,D=%.2f\r\n", meas_d);
        cdc_send_response(buf);

    } else if (result == DENSITOMETER_CAL_ERROR) {
        cdc_send_response("ERR,CAL\r\n");

    } else if (result == DENSITOMETER_SENSOR_ERROR) {
        cdc_send_response("ERR,SENSOR\r\n");

    } else {
        cdc_send_response("ERR\r\n");

    }
}

void cdc_command_measure_transmission(const char *cmd, size_t len)
{
    /*
     * "MT" : Measurement Transmission
     */

    UNUSED(cmd);
    UNUSED(len);

    densitometer_result_t result = densitometer_transmission_measure(NULL, NULL);
    if (result == DENSITOMETER_OK) {
        char buf[128];

        float meas_d = densitometer_transmission_get_last_reading();
        sprintf_(buf, "MT,D=%.2f\r\n", meas_d);
        cdc_send_response(buf);

    } else if (result == DENSITOMETER_CAL_ERROR) {
        cdc_send_response("ERR,CAL\r\n");

    } else if (result == DENSITOMETER_SENSOR_ERROR) {
        cdc_send_response("ERR,SENSOR\r\n");

    } else {
        cdc_send_response("ERR\r\n");

    }
}

void cdc_command_cal_gain(const char *cmd, size_t len)
{
    /*
     * "CG" : Calibration Gain
     * "CGM" -> Measure actual sensor gain and save as calibration values
     * "CGP" -> Return currently saved gain calibration values
     */

    if (len < 3) {
        return;
    }

    char prefix = toupper(cmd[2]);

    if (prefix == 'M') {
        if (sensor_gain_calibration(NULL, NULL) == osOK) {
            cdc_send_response("OK\r\n");
        } else {
            cdc_send_response("ERR\r\n");
        }
    } else if (prefix == 'P') {
        char buf[128];
        float ch0_gain, ch1_gain;

        settings_get_cal_gain(TSL2591_GAIN_MEDIUM, &ch0_gain, &ch1_gain);
        sprintf_(buf, "TSL2591,MEDIUM,%.2f,%.2f\r\n", ch0_gain, ch1_gain);
        cdc_send_response(buf);

        settings_get_cal_gain(TSL2591_GAIN_HIGH, &ch0_gain, &ch1_gain);
        sprintf_(buf, "TSL2591,HIGH,%.2f,%.2f\r\n", ch0_gain, ch1_gain);
        cdc_send_response(buf);

        settings_get_cal_gain(TSL2591_GAIN_MAXIMUM, &ch0_gain, &ch1_gain);
        sprintf_(buf, "TSL2591,MAXIMUM,%.2f,%.2f\r\n", ch0_gain, ch1_gain);
        cdc_send_response(buf);
    }
}

void cdc_command_cal_light(const char *cmd, size_t len)
{
    /*
     * "CL" : Calibration Light Source
     * "CLR" -> Measure reflection light source drop over time and save as calibration values
     * "CLT" -> Measure transmission light source drop over time and save as calibration values
     * "CLP" -> Return currently saved light source calibration values
     */

    if (len < 3) {
        return;
    }

    char prefix = toupper(cmd[2]);

    if (prefix == 'R') {
        osStatus_t ret = osOK;
        ret = sensor_light_calibration(SENSOR_LIGHT_REFLECTION, NULL, NULL);
        if (ret == osOK) {
            char buf[64];
            float value;
            settings_get_cal_reflection_led_factor(&value);
            sprintf_(buf, "LIGHT,REFL,%f\r\n", value);
            cdc_send_response(buf);
            cdc_send_response("OK\r\n");
        } else {
            cdc_send_response("ERR\r\n");
        }
    } else if (prefix == 'T') {
        osStatus_t ret = osOK;
        ret = sensor_light_calibration(SENSOR_LIGHT_TRANSMISSION, NULL, NULL);
        if (ret == osOK) {
            char buf[64];
            float value;
            settings_get_cal_transmission_led_factor(&value);
            sprintf_(buf, "LIGHT,TRAN,%f\r\n", value);
            cdc_send_response(buf);
            cdc_send_response("OK\r\n");
        } else {
            cdc_send_response("ERR\r\n");
        }
    } else if (prefix == 'P') {
        char buf[64];
        float value;

        settings_get_cal_reflection_led_factor(&value);
        sprintf_(buf, "LIGHT,REFL,%f\r\n", value);
        cdc_send_response(buf);

        settings_get_cal_transmission_led_factor(&value);
        sprintf_(buf, "LIGHT,TRAN,%f\r\n", value);
        cdc_send_response(buf);

        cdc_send_response("OK\r\n");
    }
}

void cdc_command_cal_reflection(const char *cmd, size_t len)
{
    /*
     * "CRL" : Reflection Calibration CAL-LO
     * "CRLMnnn" -> Calibrate using low target with a known density of n.nn (e.g. 123 = 1.23)
     * "CLP"    -> Return currently saved low target calibration values
     *
     * "CRH" : Reflection Calibration CAL-HI
     * "CRHMnnn" -> Calibrate using high target with a known density of n.nn (e.g. 123 = 1.23)
     * "CRHP"    -> Return currently saved high target calibration values
     */

    if (len < 4) {
        return;
    }

    char mode = toupper(cmd[2]);
    char prefix = toupper(cmd[3]);

    if (mode != 'L' && mode != 'H') {
        return;
    }

    if (prefix == 'M') {
        densitometer_result_t result = DENSITOMETER_OK;

        int val = (len > 4) ? atoi(cmd + 4) : 0;

        float d = val / 100.0F;
        if (d < 0) { d = 0.0F; }
        else if (d > REFLECTION_MAX_D) { d = REFLECTION_MAX_D; }

        float meas_value = NAN;

        if (mode == 'L') {
            result = densitometer_calibrate_reflection_lo(d, NULL, NULL);
            settings_get_cal_reflection_lo(NULL, &meas_value);
        } else if (mode == 'H') {
            result = densitometer_calibrate_reflection_hi(d, NULL, NULL);
            settings_get_cal_reflection_hi(NULL, &meas_value);
        } else {
            result = DENSITOMETER_SENSOR_ERROR;
        }

        if (result == DENSITOMETER_OK) {
            log_i("CAL, %c, D=%.2f, VALUE=%f", mode, d, meas_value);

            cdc_send_response("OK\r\n");
        } else {
            cdc_send_response("ERR\r\n");
        }

        sensor_set_light_mode(SENSOR_LIGHT_OFF, false, 0);

    } else if (prefix == 'P') {
        char buf[128];
        float d = 0;
        float value = 0;

        if (mode == 'L') {
            settings_get_cal_reflection_lo(&d, &value);
        } else if (mode == 'H') {
            settings_get_cal_reflection_hi(&d, &value);
        }

        sprintf_(buf, "CAL,%c,%.2f,%f\r\n", mode, d, value);
        cdc_send_response(buf);
    }
}

void cdc_command_cal_transmission(const char *cmd, size_t len)
{
    /*
     * "CTZ" : Transmission Calibration Zero
     * "CTZM" -> Calibrate the zero target, which is when no film is in the sensor path
     * "CTZP" -> Return currently saved zero target calibration value
     *
     * "CTH" : Transmission Calibration CAL-HI
     * "CTHMnnn" -> Calibrate using high target with a known density of n.nn (e.g. 123 = 1.23)
     * "CTHP"    -> Return currently saved high target calibration values
     */

    char mode = toupper(cmd[2]);
    char prefix = toupper(cmd[3]);

    if (mode != 'Z' && mode != 'H') {
        return;
    }

    if (prefix == 'M') {
        densitometer_result_t result = DENSITOMETER_OK;
        float d = NAN;
        float meas_value = NAN;

        if (mode == 'H') {
            int val = (len > 4) ? atoi(cmd + 4) : 0;
            d = val / 100.0F;
            if (d < 0) { d = 0.0F; }
        }

        if (mode == 'Z') {
            result = densitometer_calibrate_transmission_zero(NULL, NULL);
            settings_get_cal_transmission_zero(&meas_value);
        } else if (mode == 'H') {
            result = densitometer_calibrate_transmission_hi(d, NULL, NULL);
            settings_get_cal_transmission_hi(NULL, &meas_value);
        } else {
            result = DENSITOMETER_SENSOR_ERROR;
        }

        if (result == DENSITOMETER_OK) {
            log_i("CAL, %c, D=%.2f, VALUE=%f", mode, d, meas_value);

            cdc_send_response("OK\r\n");
        } else {
            cdc_send_response("ERR\r\n");
        }

        sensor_set_light_mode(SENSOR_LIGHT_OFF, false, 0);

    } else if (prefix == 'P') {
        char buf[128];
        float d = 0;
        float value = 0;

        if (mode == 'Z') {
            settings_get_cal_transmission_zero(&value);
            sprintf_(buf, "CAL,%c,%f\r\n", mode, value);
        } else if (mode == 'H') {
            settings_get_cal_transmission_hi(&d, &value);
            sprintf_(buf, "CAL,%c,%.2f,%f\r\n", mode, d, value);
        } else {
            sprintf(buf, "ERR\r\n");
        }

        cdc_send_response(buf);
    }
}

void cdc_command_diag_system(const char *cmd, size_t len)
{
    char buf[128];

    /*
     * "DA" : Diagnostic About System
     * "DAI" -> Device information (HAL version, MCU Rev ID, MCU Dev ID, SysClock)
     * "DAU" -> Device unique ID (MCU unique ID)
     * "DAF" -> FreeRTOS runtime information
     * "DAS" -> Internal sensor readings
     */

    if (len < 3) {
        return;
    }

    char prefix = toupper(cmd[2]);

    if (prefix == 'I') {
        uint32_t hal_ver = HAL_GetHalVersion();
        uint8_t hal_ver_code = ((uint8_t)(hal_ver)) & 0x0F;

        /*
         * Output format:
         * HAL Version, MCU Revision ID, MCU Device ID, SysClock Frequency
         */

        sprintf(buf, "%d.%d.%d%c,0x%lX,0x%lX,%ldMHz\r\n",
            ((uint8_t)(hal_ver >> 24)) & 0x0F,
            ((uint8_t)(hal_ver >> 16)) & 0x0F,
            ((uint8_t)(hal_ver >> 8)) & 0x0F,
            (hal_ver_code > 0 ? (char)hal_ver_code : ' '),
            HAL_GetREVID(),
            HAL_GetDEVID(),
            HAL_RCC_GetSysClockFreq() / 1000000);
        cdc_send_response(buf);

    } else if (prefix == 'U') {
        uint8_t *uniqueId = (uint8_t*)UID_BASE;

        sprintf(buf, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
            uniqueId[0], uniqueId[1], uniqueId[2], uniqueId[3], uniqueId[4],
            uniqueId[5], uniqueId[6], uniqueId[7], uniqueId[8], uniqueId[9],
            uniqueId[10], uniqueId[11]);
        cdc_send_response(buf);
    } else if (prefix == 'F') {
        sprintf(buf, "FreeRTOS,%s\r\n", tskKERNEL_VERSION_NUMBER);
        cdc_send_response(buf);

        sprintf(buf, "HEAP,%d,%d\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
        cdc_send_response(buf);

        TaskStatus_t *pxTaskStatusArray;
        volatile UBaseType_t uxArraySize, x;
        uxArraySize = uxTaskGetNumberOfTasks();
        pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
        if (pxTaskStatusArray != NULL) {
            uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, NULL);
            for (x = 0; x < uxArraySize; x++) {
                sprintf(buf, "TASK,\"%s\",%d\r\n",
                    pxTaskStatusArray[x].pcTaskName,
                    pxTaskStatusArray[x].usStackHighWaterMark * 4);
                cdc_send_response(buf);
            }
            vPortFree(pxTaskStatusArray);
        }
        cdc_send_response("OK\r\n");
    } else if (prefix == 'S') {
        adc_readings_t readings;
        if (adc_read(&readings) == osOK) {
            sprintf(buf, "VDDA,%dmV\r\n", readings.vdda_mv);
            cdc_send_response(buf);

            sprintf(buf, "TEMP,%dC\r\n", readings.temp_c);
            cdc_send_response(buf);

            cdc_send_response("OK\r\n");
        } else {
            cdc_send_response("ERR\r\n");
        }
    }
}

void cdc_command_diag_display(const char *cmd, size_t len)
{
    /*
     * "DD" : Diagnostic Display
     * "DDS" -> Capture display screenshot
     */

    if (len < 3) {
        return;
    }

    char prefix = toupper(cmd[2]);

    if (prefix == 'S') {
        display_capture_screenshot();
    }
}

void cdc_command_diag_light(const char *cmd, size_t len)
{
    /*
     * "DL" : Diagnostic Light
     * "DLRnnn" -> Reflection light, duty cycle = nnn/127
     * "DLTnnn" -> Transmission light, duty cycle = nnn/127
     * "DLAnnn" -> All light sources, duty cycle = nnn/127
     */

    if (len < 3) {
        return;
    }

    char prefix = toupper(cmd[2]);

    uint8_t val = (len > 3) ? atoi(cmd + 3) : 0;
    if (val > 128) { val = 128; }

    if (prefix == 'R') {
        log_d("Set reflection light to %d", val);
        light_set_reflection(val);
    } else if (prefix == 'T') {
        log_d("Set transmission light to %d", val);
        light_set_transmission(val);
    } else if (prefix == 'A') {
        log_d("Set all light sources to %d", val);
        light_set_reflection(val);
        light_set_transmission(val);
    }
}

void cdc_command_diag_sensor(const char *cmd, size_t len)
{
    /*
     * "DS" : Diagnostic Sensor
     * "DSR"  -> Take reading (output format TBD)
     * "DSSnm" -> Set gain (n = [0-3]) and integration time (m = [0-5])
     * "DSQ"  -> Query current sensor settings (output format TBD)
     */

    if (len < 3) {
        return;
    }

    char prefix = toupper(cmd[2]);

    if (prefix == 'R') {
        log_d("Take sensor reading");

        /*
         * Note: This should be replaced with a call to a proper sensor
         * handling routine, once one is written. For now, it is just
         * directly calling the sensor's API in a rudimentary fashion.
         */

        HAL_StatusTypeDef ret;
        bool enabled = false;
        bool valid = false;
        uint16_t ch0_val = 0;
        uint16_t ch1_val = 0;
        tsl2591_gain_t gain = TSL2591_GAIN_LOW;
        tsl2591_time_t time = TSL2591_TIME_100MS;

        do {
            ret = tsl2591_enable(&hi2c1);
            if (ret != HAL_OK) { break; }
            enabled = true;

            ret = tsl2591_get_config(&hi2c1, &gain, &time);
            if (ret != HAL_OK) { break; }

            do {
                ret = tsl2591_get_status_valid(&hi2c1, &valid);
            } while (!valid && ret == HAL_OK);
            if (ret != HAL_OK) { break; }

            ret = tsl2591_get_full_channel_data(&hi2c1, &ch0_val, &ch1_val);
            if (ret != HAL_OK) { break; }
        } while (0);

        if (enabled) {
            tsl2591_disable(&hi2c1);
        }

        if (ret == HAL_OK) {
            float ch0_basic;
            float ch1_basic;
            char buf[128];

            sensor_reading_t reading = {
                .gain = gain,
                .time = time,
                .ch0_val = ch0_val,
                .ch1_val = ch1_val
            };

            sensor_convert_to_basic_counts(&reading, &ch0_basic, &ch1_basic);

            sprintf_(buf, "TSL2591,%d,%d,%f,%f\r\n", ch0_val, ch1_val, ch0_basic, ch1_basic);
            cdc_send_response(buf);
            log_d("TSL2591: CH0=%d, CH1=%d, %f, %f", ch0_val, ch1_val, ch0_basic, ch1_basic);
        } else {
            cdc_send_response("ERR\r\n");
        }

    } else if (prefix == 'S' && len > 4 &&
        isdigit((unsigned char)cmd[3]) && isdigit((unsigned char)cmd[4])) {
        uint8_t val_g = cmd[3] - '0';
        uint8_t val_t = cmd[4] - '0';

        if (val_g > 3) { val_g = 3; }
        if (val_t > 5) { val_t = 5; }

        log_d("Set sensor gain to [%d] and integration time to [%d]", val_g, val_t);

        if (tsl2591_set_config(&hi2c1, val_g, val_t) == HAL_OK) {
            cdc_send_response("OK\r\n");
        } else {
            cdc_send_response("ERR\r\n");
        }

    } else if (prefix == 'Q') {
        tsl2591_gain_t gain;
        tsl2591_time_t time;

        log_d("Query sensor settings");

        if (tsl2591_get_config(&hi2c1, &gain, &time) == HAL_OK) {
            char buf[64];
            sprintf(buf, "TSL2591,%d,%d\r\n", gain, time);
            cdc_send_response(buf);
            log_d("TSL2591: gain=%d, time=%d", gain, time);
        } else {
            cdc_send_response("ERR\r\n");
        }
    }
}

void cdc_send_response(const char *str)
{
    size_t len = strlen(str);
    cdc_write(str, len);
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
                log_w("Write error");
                break;
            }
            tud_cdc_write_flush();
            offset += n;

            if (osSemaphoreAcquire(cdc_tx_semaphore, CDC_TX_TIMEOUT) != osOK) {
                log_e("Unable to acquire cdc_tx_semaphore");
                tud_cdc_write_clear();
                tud_cdc_abort_transfer();
                cdc_host_connected = false;
                break;
            }
        } while (offset < len);
    }
    osMutexRelease(cdc_mutex);
}
