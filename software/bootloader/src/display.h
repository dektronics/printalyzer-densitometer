#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stddef.h>

void display_init();

void display_enable();

void display_show_usb_logo();

void display_show_update_icon();

void display_show_progress(uint8_t progress);

#endif /* DISPLAY_H */
