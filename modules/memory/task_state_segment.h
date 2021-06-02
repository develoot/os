#ifndef MEMORY_TASK_STATE_SEGMENT_H
#define MEMORY_TASK_STATE_SEGMENT_H

#include <stdint.h>

/**
 * A data structure for task state segment descriptors.
 *
 * Attribute field of this data structure can be interpreted using same `SEGMENT_ATTRIBUTE_#` of the
 * application segment descriptors.
 *
 * Base address of the segment is calculated combining `address#` members.
 *
 * Segment limit is calculated combining `limit` member and `SEGMENT_ATTRIBUTE_LIMIT` field of the
 * `attribute` member.
 *
 * Note that the `SEGMENT_ATTRIBUTE_S` bit always need to be unset becaus this is system segment.
 */
struct task_state_segment_descriptor {
    uint16_t limit;
    uint16_t address0;
    uint8_t  address1;
    uint16_t attribute;
    uint8_t  address2;
    uint32_t address3;
    uint32_t reserved;
} __attribute__((packed));

/**
 * A data structure that represents actual task state segment.
 *
 * In IA-32e mode, this structure is only used for IST because hardware task management is disabled.
 */
struct task_state_segment {
    uint32_t reserved0;
    /** This field is not used in IA-32e IST operations. */
    uint64_t rsp[3];
    uint64_t reserved1;
    uint64_t interrupt_stack_table[7];
    uint32_t reserved2;
    uint32_t reserved3;
    uint16_t reserved4;
    uint16_t io_bitmap_base;
} __attribute__((packed));

#endif
