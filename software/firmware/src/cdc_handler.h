#ifndef CDC_HANDLER_H
#define CDC_HANDLER_H

#include <stddef.h>

void cdc_write(const char *buf, size_t len);
void cdc_task();

#endif /* CDC_TASK_H */
