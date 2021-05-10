#ifndef _TASK_STATE_SEGMENT_TYPE_H
#define _TASK_STATE_SEGMENT_TYPE_H

#include <stdint.h>

struct task_state_segment_descriptor {
    uint16_t limit;
    uint16_t address0;
    uint8_t  address1;
    uint16_t attribute;
    uint8_t  address2;
    uint32_t address3;
    uint32_t reserved;
} __attribute__((packed));

struct task_state_segment {
    uint32_t reserved1;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved2;
    uint64_t interrupt_stack_table[7];
    uint32_t reserved3;
    uint32_t reserved4;
    uint16_t reserved5;
    uint16_t io_bitmap_base;
} __attribute__((packed));

#endif
