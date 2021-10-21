#ifndef CDC_HANDLER_H
#define CDC_HANDLER_H

#include <stddef.h>
#include <stdbool.h>

void task_cdc_run(void *argument);

/**
 * Send a density reading out the CDC device.
 *
 * @param prefix The reading type, such as 'R' or 'T'
 * @param d_value The density reading value
 * @param raw_value The raw sensor reading, in basic counts
 * @param corr_value The slope corrected sensor reading, in basic counts
 */
void cdc_send_density_reading(char prefix, float d_value, float raw_value, float corr_value);

/**
 * Send a message indicating the remote control state being changed
 *
 * @param enabled True if enabled, false if disabled
 */
void cdc_send_remote_state(bool enabled);

/**
 * Write a message out the CDC device.
 *
 * This function will only sent data if the device is connected, and will
 * block until that data is sent. As such, only short bits of data should
 * be sent this way.
 *
 * It is also advisable to end each sent string with a CRLF, so it will
 * appear as expected on the receiver.
 *
 * @param buf Data to send
 * @param len Length of the data to send
 */
void cdc_write(const char *buf, size_t len);

#endif /* CDC_TASK_H */
