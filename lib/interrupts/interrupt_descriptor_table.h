#ifndef _INTERRUPT_DESCRIPTOR_TABLE_H
#define _INTERRUPT_DESCRIPTOR_TABLE_H

#include <stdint.h>

/**
 * A data structure to be loaded into IDTR register.
 *
 * The IDTR register contains this data structure and always points to the interrupt descriptor
 * table of the system.
 */
struct interrupt_descriptor_table_register_entry {
    /**
     * Specifies size of the interrupt descriptor table in byte.
     *
     * As with segments, the limit value is added to the base address to get the address of the last
     * valid byte. A limit value of 0 results in exactly one valid byte.
     *
     * Because segment descriptors are always 8 bytes long, the IDT limit should always be one less
     * than an integral multiple of eight (that is 8N - 1).
     */
    uint16_t table_limit;
    /** Specifies the base address of the interrupt descriptor table. */
    uint64_t table_address;
} __attribute__((packed));

/**
 * A data structure that represents interrupt or trap gate descriptor.
 *
 * An entry point of procedure is calculated combining `offset#` members.
 */
struct interrupt_gate_descriptor {
    uint16_t offset0;
    /** Segment selector for destination code segment. */
    uint16_t segment_selector;
    uint16_t attribute;
    uint16_t offset1;
    uint32_t offset2;
    uint64_t reserved;
} __attribute__((packed));

/**
 * Interrupt stack table index.
 *
 * An index to select one of the interrupt stack table addresses in the task state segment.
 *
 * @see struct task_state_segment
 */
#define INTERRUPT_GATE_DESCRIPTOR_ATTRIBUTE_IST  (0x0007)
/**
 * Interrupt gate descriptor type field.
 *
 * This field specifies a type of the interrupt gate pointed by this descriptor.
 *
 * There are three kinds of interrupt gate descriptor:
 * One for task gate, one for interrupt gate, and one for trap gate.
 *
 * When this field is 0b0101, this descriptor is for the task gate.
 * Note that we don't use this in IA-32e mode because hardware task management is disabled.
 *
 * When this field is 0bD110 or 0bD111, this descriptor is for interrupt gate or trap gate
 * respectively. The `D` bit indicates whether the gate is 32-bit (set) or 16-bit (unset).
 *
 * I can't find any doucments describing behavior of this bit in IA-32e mode.
 * So I just assume that in IA-32e mode, it's for 64-bit when it's set.
 */
#define INTERRUPT_GATE_DESCRIPTOR_ATTRIBUTE_TYPE (0x0F00)
#define INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT (0x0E00)
#define INTERRUPT_GATE_DESCRIPTOR_TYPE_TRAP      (0x0F00)
/**
 * Descriptor privilege level.
 *
 * This flag behaves same as the one in the application segment descriptors.
 *
 * @see SEGMENT_ATTRIBUTE_DPL
 */
#define INTERRUPT_GATE_DESCRIPTOR_ATTRIBUTE_DPL  (0x6000)
/**
 * Segment present flag.
 *
 * Indicates whether the segment is present in memory (set) or not present (clear).
 *
 * This flag behaves same as the one in the application segment descriptors.
 *
 * @see SEGMENT_ATTRIBUTE_P
 */
#define INTERRUPT_GATE_DESCRIPTOR_ATTRIBUTE_P    (0x8000)

#define interrupt_gate_descriptor_attribute(IST, Type, DPL) \
    ((uint16_t)((IST << 1) | (TYPE << 8) | (DPL << 13) | (INTERRUPT_GATE_DESCRIPTOR_ATTRIBUTE_P)))

/**
 * The data structure which holds all interrupt gate descriptors.
 *
 * The IDTR register should point to base address of this data structure on memory.
 */
struct interrupt_descriptor_table {
    struct interrupt_gate_descriptor divide_error;
    struct interrupt_gate_descriptor debug;
    struct interrupt_gate_descriptor non_maskable_interrupt;
    struct interrupt_gate_descriptor break_point;
    struct interrupt_gate_descriptor overflow;
    struct interrupt_gate_descriptor bound_range_exceeded;
    struct interrupt_gate_descriptor invalid_opcode;
    struct interrupt_gate_descriptor device_not_available;
    struct interrupt_gate_descriptor double_fault;
    struct interrupt_gate_descriptor coprocessor_segment_overrun;
    struct interrupt_gate_descriptor invalid_task_state_segment;
    struct interrupt_gate_descriptor segment_not_present;
    struct interrupt_gate_descriptor stack_full;
    struct interrupt_gate_descriptor general_protection;
    struct interrupt_gate_descriptor page_fault;
    struct interrupt_gate_descriptor x87_fpu_floating_point_error;
    struct interrupt_gate_descriptor alignment_check;
    struct interrupt_gate_descriptor machine_check;
    struct interrupt_gate_descriptor simd_floating_point;
    struct interrupt_gate_descriptor virtualization;
} __attribute__((packed, aligned(0x1000)));

#endif
