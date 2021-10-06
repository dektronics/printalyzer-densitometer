#ifndef __ELOG_PORT_H__
#define __ELOG_PORT_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*elog_port_output_callback_t)(const char *log, size_t size);

void elog_port_redirect(elog_port_output_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif /* __ELOG_PORT_H__ */
