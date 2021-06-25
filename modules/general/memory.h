#ifndef _GENERAL_MEMORY_H
#define _GENERAL_MEMORY_H

#include <stdint.h>

typedef uint8_t byte_t;

static inline void memory_copy(void *const destination, const void *const source, uint64_t size)
{
    byte_t *const dst = (byte_t *)destination;
    const byte_t *const src = (byte_t *)source;

    for (uint64_t i = 0; i < size; ++i) {
        dst[i] = src[i];
    }
}

static inline int memory_compare(const void *const first, const void *const second, uint64_t size)
{
    const byte_t *const frst = (byte_t *)first;
    const byte_t *const scnd = (byte_t *)second;

    for (uint64_t i = 0; i < size; ++i) {
        if (frst[i] != scnd[i]) {
            return -1;
        }
    }

    return 0;
}

#endif
