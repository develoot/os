#ifndef _MEMORY_SEGMENT_SELECTOR_H
#define _MEMORY_SEGMENT_SELECTOR_H

#include <stdint.h>

/**
 * A segment selector.
 *
 * @param Index Selects one of descriptors in the GDT or LDT.
 *
 * @param TI Specifies the descriptor table to use: clearing this flag selects the GDT;
 * setting the flag selects the current LDT.
 *
 * @param RPL Specifies the privilege level of the selector. The privilege level can
 * range from 0 to 3, with 0 being the most privileged level.
 */
#define segment_selector(RPL, TI, Index) \
    ((uint16_t)((RPL) | ((TI) << 2) | ((Index) << 3)))

#endif
