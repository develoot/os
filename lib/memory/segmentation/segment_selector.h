#ifndef _MEMORY_SEGMENT_SELECTOR_H
#define _MEMORY_SEGMENT_SELECTOR_H

/**
 * A segment selector.
 *
 * @param Index Selects one of descriptors in the GDT or LDT.
 *
 * @param TableIndicator Specifies the descriptor table to use: clearing this flag selects the GDT;
 * setting the flag selects the current LDT.
 *
 * @param PrivilegeLevel Specifies the privilege level of the selector. The privilege level can
 * range from 0 to 3, with 0 being the most privileged level.
 */
#define segment_selector(Index, TableIndicator, PrivilegeLevel) \
    ((PrivilegeLevel) | ((TableIndicator) << 2) | (Index))

#endif
