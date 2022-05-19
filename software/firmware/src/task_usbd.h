/*
 * USBD task that handles the TinyUSB device stack, and any related
 * callbacks that are not implemented elsewhere.
 */
#ifndef TASK_USBD_H
#define TASK_USBD_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Start the USBD task.
 *
 * @param argument The osSemaphoreId_t used to synchronize task startup.
 */
void task_usbd_run(void *argument);

/**
 * Get whether the USB HID device is ready for use.
 */
bool usb_hid_ready();

/**
 * Forcibly disconnect and reconnect the device from the host.
 *
 * Note: This function will block for ~20ms during the reconnect
 * cycle, so it should only be called from contexts where that
 * behavior is okay.
 */
void usb_device_reconnect();

/**
 * Send a string of text as keystrokes out the HID device.
 *
 * @param str String of text to send
 * @param len Length of the string of text
 */
void usbd_hid_send(const char *str, size_t len);

#endif /* TASK_USBD_H */
