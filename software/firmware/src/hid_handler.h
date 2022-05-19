/*
 * Support functions to act as a front-end for the USB HID device.
 */
#ifndef HID_HANDLER_H
#define HID_HANDLER_H

/**
 * Send a density reading out the HID device.
 *
 * @param prefix The reading type, such as 'R' or 'T'
 * @param d_value The density reading value
 * @param d_zero The density "zero" offset
 */
void hid_send_density_reading(char prefix, float d_value, float d_zero);

#endif /* HID_HANDLER_H */
