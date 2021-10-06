#ifndef CDC_HANDLER_H
#define CDC_HANDLER_H

#include <stddef.h>

void task_cdc_run(void *argument);

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
