#include "app_descriptor.h"

const __attribute__((section(".app_descriptor"))) app_descriptor_t app_descriptor = {
    .magic_word = APP_DESCRIPTOR_MAGIC_WORD,
    .project_name = "Printalyzer Densitometer",
    .version = "v0.3.0",
    .build_date = APP_BUILD_DATE,
    .build_describe = APP_BUILD_DESCRIBE,
    .crc32 = 0xFFFFFFFF /* This is overwritten by the build process */
};

#ifndef __CDT_PARSER__
_Static_assert(sizeof(APP_BUILD_DATE) <= sizeof(app_descriptor.build_date), "APP_BUILD_DATE is longer than version field in structure");
_Static_assert(sizeof(APP_BUILD_DESCRIBE) <= sizeof(app_descriptor.build_describe), "APP_BUILD_DESCRIBE is longer than version field in structure");
#endif

const app_descriptor_t *app_descriptor_get()
{
    return &app_descriptor;
}
