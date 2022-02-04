/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ha Thach for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -------------------------------------------------------------------------
 * Note: Board support code has been extensively modified from the initial
 * version provided by the TinyUF2 project.
 */

#ifndef BOARDS_H
#define BOARDS_H

#include "stm32l0xx_hal.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "board.h"

//--------------------------------------------------------------------+
// Features
//--------------------------------------------------------------------+

// Flash Start Address of Application
#ifndef BOARD_FLASH_APP_START
#define BOARD_FLASH_APP_START  0
#endif

//--------------------------------------------------------------------+
// Basic API
//--------------------------------------------------------------------+

/**
 * Send characters to UART for debugging
 */
void board_uart_write_str(const char *msg);

/**
 * Send a number to UART for debugging
 */
void board_uart_write_hex(uint32_t value);

/**
 * Check if application is valid
 */
bool board_app_valid(void);

/**
 * Jump to Application
 */
void board_app_jump(void);

/**
 * DFU is complete, should reset or jump to application mode and not return
 */
void board_dfu_complete(void);

/**
 * Fill Serial Number and return its length (limit to 16 bytes)
 */
uint8_t board_usb_get_serial(uint8_t *serial_id);

//--------------------------------------------------------------------+
// Flash API
//--------------------------------------------------------------------+

/**
 * Initialize flash for DFU
 */
void board_flash_init(void);

/**
 * Get size of flash
 */
uint32_t board_flash_size(void);

/**
 * Read from flash
 */
void board_flash_read(uint32_t addr, uint8_t *buffer, uint32_t len);

/**
 * Write to flash
 */
void board_flash_write(uint32_t addr, const uint8_t *data, uint32_t len);

/**
 * Flush/Sync flash contents
 */
void board_flash_flush(void);

//--------------------------------------------------------------------+
// Display API
//--------------------------------------------------------------------+

/**
 * Initialize the display hardware
 */
void board_display_init(void);

//--------------------------------------------------------------------+
// LOG
//--------------------------------------------------------------------+
#if TUF2_LOG

#include <stdio.h>

#ifndef tuf2_printf
#define tuf2_printf printf
#endif

// Log with debug level 1
#define TUF2_LOG1               tuf2_printf
#define TUF2_LOG1_MEM           // tu_print_mem
#define TUF2_LOG1_VAR(_x)       // tu_print_var((uint8_t const*)(_x), sizeof(*(_x)))
#define TUF2_LOG1_INT(_x)       tuf2_printf(#_x " = %ld\n", (uint32_t) (_x) )
#define TUF2_LOG1_HEX(_x)       tuf2_printf(#_x " = %lX\n", (uint32_t) (_x) )
#define TUF2_LOG1_LOCATION()    tuf2_printf("%s: %d:\r\n", __PRETTY_FUNCTION__, __LINE__)
#define TUF2_LOG1_FAILED()      tuf2_printf("%s: %d: Failed\r\n", __PRETTY_FUNCTION__, __LINE__)

// Log with debug level 2
#if CFG_TUSB_DEBUG > 1
  #define TUF2_LOG2             TUF2_LOG1
  #define TUF2_LOG2_MEM         TUF2_LOG1_MEM
  #define TUF2_LOG2_VAR         TUF2_LOG1_VAR
  #define TUF2_LOG2_INT         TUF2_LOG1_INT
  #define TUF2_LOG2_HEX         TUF2_LOG1_HEX
  #define TUF2_LOG2_LOCATION()  TUF2_LOG1_LOCATION()
#endif

#endif // TUF2_LOG

#ifndef TUF2_LOG1
  #define TUF2_LOG1(...)
  #define TUF2_LOG1_MEM(...)
  #define TUF2_LOG1_VAR(...)
  #define TUF2_LOG1_INT(...)
  #define TUF2_LOG1_HEX(...)
  #define TUF2_LOG1_LOCATION()
  #define TUF2_LOG1_FAILED()
#endif

#ifndef TUF2_LOG2
  #define TUF2_LOG2(...)
  #define TUF2_LOG2_MEM(...)
  #define TUF2_LOG2_VAR(...)
  #define TUF2_LOG2_INT(...)
  #define TUF2_LOG2_HEX(...)
  #define TUF2_LOG2_LOCATION()
#endif

// Simpler logging macros that do not require printf or va_args
#ifdef HAL_UART_MODULE_ENABLED
  #define BL_LOG_STR(_x) board_uart_write_str(_x)
  #define BL_LOG_HEX(_x) board_uart_write_hex(_x)
#else
  #define BL_LOG_STR(_x)
  #define BL_LOG_HEX(_x)
#endif

//--------------------------------------------------------------------+
// not part of board API, move to its own file later
//--------------------------------------------------------------------+

enum {
    STATE_BOOTLOADER_STARTED = 0,
    STATE_USB_PLUGGED,
    STATE_USB_UNPLUGGED,
    STATE_WRITING_STARTED,
    STATE_WRITING_FINISHED
};

/**
 * Set the display to indicate a bootloader state change
 */
void indicator_set(uint32_t state);

#endif
