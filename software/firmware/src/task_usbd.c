#include "task_usbd.h"

#define LOG_TAG "task_usbd"
#include <elog.h>

#include "stm32l0xx_hal.h"
#include <stdbool.h>
#include <cmsis_os.h>
#include <tusb.h>
#include "task_main.h"
#include "usb_descriptors.h"

static volatile bool usbd_initialized = false;
static bool suspend_pending = false;

#define HID_BUFFER_LEN 32

static char hid_buffer[HID_BUFFER_LEN];
static size_t hid_buffer_len = 0;
static size_t hid_buffer_offset = 0;
static bool hid_has_key = false;

/* Conversion table for transforming ASCII into key events */
static const uint8_t hid_conv_table[128][2] =  { HID_ASCII_TO_KEYCODE };

/* Mutex used to allow access to USB state data from different tasks */
static osMutexId_t usb_mutex = NULL;
static const osMutexAttr_t usb_mutex_attrs = {
    .name = "usb_mutex"
};

static void usbd_hid_send_next_report();

void task_usbd_run(void *argument)
{
    osSemaphoreId_t task_start_semaphore = argument;

    log_d("usbd_task start");

    /* Create the HID write mutex */
    usb_mutex = osMutexNew(&usb_mutex_attrs);
    if (!usb_mutex) {
        log_e("Unable to create USB mutex");
        return;
    }

    /* Initialize the TinyUSB stack */
    if (!tusb_init()) {
        log_e("Unable to initialize tusb");
        return;
    }

    /* Enable USB interrupt */
    HAL_NVIC_EnableIRQ(USB_IRQn);

    /* Enable EXTI Line 18 for USB wakeup */
    __HAL_USB_WAKEUP_EXTI_ENABLE_IT();

    USB->LPMCSR |= USB_LPMCSR_LMPEN;
    USB->LPMCSR |= USB_LPMCSR_LPMACK;

    /* Release the startup semaphore */
    if (osSemaphoreRelease(task_start_semaphore) != osOK) {
        log_e("Unable to release task_start_semaphore");
        return;
    }

    usbd_initialized = true;

    /* Run TinyUSB device task */
    while (1) {
        tud_task();

        if (task_main_is_running() && suspend_pending) {
            log_d("Deferred suspend");
            suspend_pending = false;
            task_main_force_state(STATE_SUSPEND);
        }
    }
}

/**
 * Invoked when the device is mounted (configured)
 */
void tud_mount_cb()
{
    log_d("tud_mount_cb");
}

/**
 * Invoked when the device is unmounted
 */
void tud_umount_cb()
{
    log_d("tud_umount_cb");
}

/**
 * Invoked when the USB device is suspended.
 *
 * Within 7ms, the device must drop its current draw to an average of
 * less than 2.5mA.
 */
void tud_suspend_cb(bool remote_wakeup_en)
{
    UNUSED(remote_wakeup_en);
    log_d("tud_suspend_cb");

    if (task_main_is_running()) {
        /* Force the main task to enter its suspend state */
        task_main_force_state(STATE_SUSPEND);
    } else {
        suspend_pending = true;
    }
}

/**
 * Invoked when the USB device is resumed.
 */
void tud_resume_cb()
{
    log_d("tud_resume_cb");

    suspend_pending = false;

    if (task_main_is_running()) {
        /* Force the main task to exit its suspend state */
        task_main_force_state(STATE_HOME);
    }
}

/**
 * Invoked when a REPORT has been successfully sent to the host
 *
 * Application can use this to send the next report.
 * Note: For composite reports, report[0] is report ID.
 */
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
    (void) instance;
    (void) len;

    if (!usbd_initialized) { return; }

    osMutexAcquire(usb_mutex, portMAX_DELAY);

    usbd_hid_send_next_report();

    osMutexRelease(usb_mutex);
}

/**
 * Invoked when receiving a GET_REPORT control request.
 *
 * Application must fill buffer report's content and return its length.
 * Returning zero will cause the stack to STALL request.
 */
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    /* Not implemented */
    log_d("tud_hid_get_report_cb");

    return 0;
}

/**
 * Invoked when receiving a SET_REPORT control request or received data
 * on the OUT endpoint ( Report ID = 0, Type = 0)
 */
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;

    /*
     * Normally this callback deals with stateful items like capslock
     * and numlock, which are not relevant to this device.
     */
    log_d("tud_hid_set_report_cb");
}

bool usb_hid_ready()
{
    return usbd_initialized && tud_hid_ready() && !suspend_pending;
}

void usb_device_reconnect()
{
    if (!usbd_initialized || suspend_pending || !tud_connected()) { return; }
    log_d("Triggering explicit reconnect");

    osMutexAcquire(usb_mutex, portMAX_DELAY);
    tud_disconnect();
    osDelay(10);
    tud_connect();
    osDelay(10);
    osMutexRelease(usb_mutex);
}

void usbd_hid_send(const char *str, size_t len)
{
    /* Skip if HID is not ready yet */
    if (!tud_hid_ready() || suspend_pending) { return; }

    osMutexAcquire(usb_mutex, portMAX_DELAY);

    /* Prevent overwriting existing send-in-progress */
    if (hid_buffer_len > 0 && hid_buffer_offset > 0) {
        log_w("HID send already in progress");
        osMutexRelease(usb_mutex);
        return;
    }

    /* Clear out the buffer and reset state variables */
    hid_buffer_len = 0;
    hid_buffer_offset = 0;
    bzero(hid_buffer, sizeof(hid_buffer));
    hid_has_key = false;

    /* Iterate through the input string, and add HID-supported characters */
    for (size_t i = 0; i < len; i++) {
        char ch = str[i];
        uint8_t keycode = hid_conv_table[(size_t)ch][1];
        if (keycode > 0) {
            hid_buffer[hid_buffer_len++] = ch;
        }
    }

    /* Start the HID report sending process */
    usbd_hid_send_next_report();

    osMutexRelease(usb_mutex);
}

void usbd_hid_send_next_report()
{
    if (hid_buffer_offset < hid_buffer_len) {
        if (hid_has_key) {
            /* Send the key-up event and advance in the buffer */
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, 0);
            hid_buffer_offset++;
            hid_has_key = false;

            /* Reset the buffer state if the end has been reached */
            if (hid_buffer_offset == hid_buffer_len) {
                hid_buffer_len = 0;
                hid_buffer_offset = 0;
            }
        } else {
            uint8_t keycode[6] = { 0 };
            uint8_t modifier = 0;
            char ch = hid_buffer[hid_buffer_offset];

            /* Convert the next character into a keycode and modifier */
            if (hid_conv_table[(size_t)ch][0]) { modifier = KEYBOARD_MODIFIER_LEFTSHIFT; }
            keycode[0] = hid_conv_table[(size_t)ch][1];

            /* Send the key-down event for the next character */
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, keycode);
            hid_has_key = true;
        }
    }
}
