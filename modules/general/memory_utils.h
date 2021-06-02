#ifndef _GENERAL_MEMORY_UTILS_H
#define _GENERAL_MEMORY_UTILS_H

#include <stdint.h>
#include <general/inline.h>

always_inline void memory_copy(uint8_t *const destination, const uint8_t *source, uint64_t size)
{
    for (uint64_t i = 0; i < size; ++i) {
        destination[i] = source[i];
    }
}

always_inline int memory_compare(const uint8_t *first, const uint8_t *second, uint64_t size)
{
    for (uint64_t i = 0; i < size; ++i) {
        if (first[i] != second[i]) {
            return -1;
        }
    }

    return 0;
}

#endif
