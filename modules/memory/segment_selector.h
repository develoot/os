#ifndef _MEMORY_SEGMENT_SELECTOR_H
#define _MEMORY_SEGMENT_SELECTOR_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Return a segment selector.
 *
 * @param segment_index Selects one of descriptors in the GDT or LDT.
 *
 * @param table_indicator Specifies the descriptor table to use: clearing this flag selects the GDT;
 * setting the flag selects the current LDT.
 *
 * @param requested_privilege_level Specifies the privilege level of the selector. The privilege
 * level can range from 0 to 3, with 0 being the most privileged level.
 */
static inline uint16_t segment_selector(uint8_t requested_privilege_level, bool table_indicator,
        uint8_t segment_index)
{
    return (uint16_t)(requested_privilege_level | table_indicator << 2 | segment_index << 3);
}

#endif
