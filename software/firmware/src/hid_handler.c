#include "hid_handler.h"

#include <printf.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "settings.h"
#include "task_usbd.h"
#include "util.h"

void hid_send_density_reading(char prefix, float d_value, float d_zero)
{
    float d_display;
    char buf[16];
    size_t offset = 0;
    size_t n;
    char sign;

    /* Abort quickly if the HID device is not ready */
    if (!usb_hid_ready()) { return; }

    /* Get the settings for display formatting */
    settings_user_display_format_t display_format;
    settings_get_user_display_format(&display_format);

    /* Get the settings for this feature */
    settings_user_usb_key_t usb_key;
    settings_get_user_usb_key(&usb_key);

    /* Abort if this feature is not enabled */
    if (!usb_key.enabled) { return; }

    /* Force any invalid values to be zero */
    if (isnanf(d_value) || isinff(d_value)) {
        d_value = 0.0F;
    }

    /* Calculate the display value */
    if (!isnanf(d_zero)) {
        d_display = d_value - d_zero;
    } else {
        d_display = d_value;
    }

    /* Convert the output if set to f-stop unit mode */
    if (display_format.unit == SETTING_DISPLAY_UNIT_FSTOP) {
        d_display = log2f(powf(10.0F, d_display));
    }

    /* Find the sign character */
    if (d_display >= 0.0F) {
        sign = '+';
    } else {
        sign = '-';
    }

    if (usb_key.format == SETTING_KEY_FORMAT_FULL) {
        /* Format the result */
        if (display_format.unit == SETTING_DISPLAY_UNIT_FSTOP) {
            n = sprintf_(buf, "%c%c%.2fF", prefix, sign, fabsf(d_display));
        } else {
            n = sprintf_(buf, "%c%c%.2fD", prefix, sign, fabsf(d_display));
        }

        /* Catch cases where a negative was rounded to zero */
        if (strncmp(buf + 1, "-0.00", 5) == 0) {
            buf[1] = '+';
        }
    } else {
        /* Format the result */
        n = sprintf_(buf, "%c%.2f", sign, fabsf(d_display));

        /* Catch cases where a negative was rounded to zero */
        if (strncmp(buf, "-0.00", 5) == 0) {
            buf[0] = '+';
        }

        /* Trim the leading sign if not negative */
        if (buf[0] == '+') {
            offset++;
        }
    }

    /* Change the decimal separator if configured to do so */
    if (display_format.separator == SETTING_DECIMAL_SEPARATOR_COMMA) {
        replace_first_char(buf, '.', ',');
    }

    /* Append the separator character */
    switch (usb_key.separator) {
    case SETTING_KEY_SEPARATOR_ENTER:
        buf[n++] = '\n'; buf[n] = '\0';
        break;
    case SETTING_KEY_SEPARATOR_TAB:
        buf[n++] = '\t'; buf[n] = '\0';
        break;
    case SETTING_KEY_SEPARATOR_COMMA:
        if (display_format.separator == SETTING_DECIMAL_SEPARATOR_COMMA) {
            buf[n++] = ';';
        } else {
            buf[n++] = ',';
        }
        buf[n] = '\0';
        break;
    case SETTING_KEY_SEPARATOR_SPACE:
        buf[n++] = ' '; buf[n] = '\0';
        break;
    case SETTING_KEY_SEPARATOR_NONE:
    default:
        break;
    }

    /* Send the formatted string to the HID interface */
    usbd_hid_send(buf + offset, n - offset);
}
