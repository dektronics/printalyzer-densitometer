#include "cdc_handler.h"

#define LOG_TAG "main"

#include "stm32l0xx_hal.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <elog.h>
#include <tusb.h>

#define CMD_DATA_SIZE 64

static uint8_t cmd_buffer[CMD_DATA_SIZE];
static size_t cmd_buffer_len = 0;

static void cdc_process_command(const char *cmd, size_t len);
static void cdc_send_response(const char *str);

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    log_d("cdc_line_state: itf=%d, dtr=%d, rts=%d", itf, dtr, rts);
}

void cdc_task()
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
        tud_cdc_write(buf, count);
        tud_cdc_write_flush();

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
        char buf[64];
        sprintf(buf, "Printalyzer Densitometer\n");
        cdc_send_response(buf);
    }
}

void cdc_send_response(const char *str)
{
    size_t len = strlen(str);
    tud_cdc_write(str, len);
    tud_cdc_write_flush();
}
