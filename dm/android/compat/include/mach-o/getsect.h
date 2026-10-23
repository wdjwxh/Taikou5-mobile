#pragma once
#include "ldsyms.h"
#include <stdint.h>
#include <string.h>
/* The same registration entries use ELF sections on Android. */
static inline uint8_t *getsectiondata(const struct mach_header_64 *header,
    const char *segment, const char *section, unsigned long *size) {
    (void)header; (void)segment;
    #define NX_SECTION(name) do { \
        extern const uint8_t __start_##name[] __attribute__((weak,visibility("hidden"))); \
        extern const uint8_t __stop_##name[] __attribute__((weak,visibility("hidden"))); \
        if (!strcmp(section,#name)) { \
            *size=__start_##name ? (unsigned long)(__stop_##name-__start_##name) : 0; \
            return (uint8_t *)__start_##name; \
        } \
    } while(0)
    NX_SECTION(swhle); NX_SECTION(swforce); NX_SECTION(swnvn);
    #undef NX_SECTION
    *size=0; return 0;
}
