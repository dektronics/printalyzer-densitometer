#ifndef CDC_HANDLER_H
#define CDC_HANDLER_H

#include <stddef.h>
#include <stdbool.h>

#include "sensor.h"

void task_cdc_run(void *argument);

/**
 * Get whether the CDC device is currently connected to a host.
 *
 * The connection state is based on the state of the DTR signal,
 * as the best available proxy for knowing if a host-side application
 * is currently connected to the device.
 *
 * Note: There may be corner cases around state transitions, but this can
 * generally be used as a way to decide whether or not to attempt
 * a write of measurement data.
 */
bool cdc_is_connected();

/**
 * Send a density reading out the CDC device.
 *
 * @param prefix The reading type, such as 'R' or 'T'
 * @param d_value The density reading value
 * @param d_zero The density "zero" offset
 * @param raw_value The raw sensor reading, in basic counts
 * @param corr_value The slope corrected sensor reading, in basic counts
 */
void cdc_send_density_reading(char prefix, float d_value, float d_zero, float raw_value, float corr_value);

/**
 * Send a message containing raw sensor data for diagnostic purposes
 *
 * @param reading Raw sensor reading data
 */
void cdc_send_raw_sensor_reading(const sensor_reading_t *reading);

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
