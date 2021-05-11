#ifndef _MEMORY_SEGMENTATION_TYPE_H
#define _MEMORY_SEGMENTATION_TYPE_H

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

#define TASK_STATE_SEGMENT_STACK_SIZE (512)

struct task_state_segment {
    uint32_t reserved1;
    /** This field is not used in IA-32e IST operations. */
    uint64_t rsp[3];
    uint64_t reserved2;
    uint64_t interrupt_stack_table[7];
    uint32_t reserved3;
    uint32_t reserved4;
    uint16_t reserved5;
    uint16_t io_bitmap_base;
} __attribute__((packed));

/** A data structure to be loaded into GDTR register. */
struct global_descriptor_table_register {
    /**
     * Specifies size of the global descriptor table in byte.
     *
     * As with segments, the limit value is added to the base address to get the address of the last
     * valid byte. A limit value of 0 results in exactly one valid byte.
     *
     * Because segment descriptors are always 8 bytes long, the GDT limit should always be one less
     * than an integral multiple of eight (that is 8N - 1).
     */
    uint16_t table_limit;
    /** Specifies start address of the global descriptor table. */
    uint64_t table_address;
} __attribute__((packed));

/**
 * A general structure for segment descriptors.
 *
 * Base address is calculated combining `address0`, `address1`, and `address2`.
 *
 * Segment limit is calculated combining `limit` and `SEGMENT_ATTRIBUTE_LIMIT` field of the
 * `attribute` member.
 *
 * Most fileds of descriptors are set to 0 as the segmentation is mostly deactivated in IA-32e mode.
 *
 * @see SEGMENT_ATTRIBUTE_LIMIT
 */
struct segment_descriptor {
    uint16_t limit;
    uint16_t address0;
    uint8_t  address1;
    /**
     * Attributes of the segment pointed by this descriptor.
     *
     * @see SEGMENT_ATTRIBUTE_#
     */
    uint16_t attribute;
    uint8_t  address2;
} __attribute__((packed));

/**
 * Type field.
 *
 * Indicates the segment or gate type and specifies the kinds of access and the direction of growth.
 *
 * Interpretation of this field depends on whether the descriptor type flag specifies an application
 * descriptor of system descriptor.
 *
 * @see Section 3.4.5.1, Intel 64 and IA-32 Software Developers Manual
 */
#define SEGMENT_ATTRIBUTE_TYPE  (0x000F)
/**
 * Descriptor type flag.
 *
 * Specifies wheter the segment descriptor is for a system segment (flag is clear) or code or data
 * segment (flag is set).
 */
#define SEGMENT_ATTRIBUTE_S     (0x0010)
/**
 * Descriptor privilege level.
 *
 * Specifies the privilege level of segment.
 *
 * The privilege level can range from 0 to 3, with 0 being the most privilege level.
 */
#define SEGMENT_ATTRIBUTE_DPL   (0x0060)
/**
 * Segment present flag.
 *
 * Indicates whether the segment is present in memory (set) or not present (clear).
 *
 * If this flag is clear, the process generates a segment-not-present exception (#NP) when a segment
 * selector that points to the segment descriptor is loaded into a segment register.
 */
#define SEGMENT_ATTRIBUTE_P     (0x0080)
/**
 * Specifies limit of segment pointed by this descriptor.
 *
 * Actual limit of the segment is calculated combining `limit` member of the descriptor and this
 * field.
 */
#define SEGMENT_ATTRIBUTE_LIMIT (0x0F00)
/**
 * Free bit for operating system.
 *
 * We don't use this bit in this operating system.
 */
#define SEGMENT_ATTRIBUTE_AVL   (0x1000)
/**
 * Indicates whether this segment contains native 64-bit code or legacy 32-bit code.
 *
 * This flag should be unset if the segment is not code segment otherwise the processor will
 * generate a general-protection exception (#GP).
 *
 * If this bit is set, then the D flag must be cleared.
 *
 * @see SEGMENT_ATTRIBUTE_DB
 */
#define SEGMENT_ATTRIBUTE_L     (0x2000)
/**
 * Default operation size / default stack pointer size / upper bound flag.
 *
 * Performs different functions depending on whether the segment descriptor is an executable code
 * segment, an expand-down data segment, or a stack segment.
 *
 * This flag should always be set for 32-bit code and data segments. For 16-bit code and data
 * segments, this flag should be cleared.
 *
 * When the descriptor points to executable code segment, this flag is called the D flag and it
 * indicates the default length for effective addresses and operands referenced by instructions in
 * the segment.
 *
 * It fhe flag is set, 32-bit addresses and 32-bit or 8-bit operands are assumed; if it is clear,
 * 16-bit addresses and 16-bit or 8-bit operands are assumed.
 *
 * This flag should be clreaed if L flag is set.
 *
 * @see SEGMENT_ATTRIBUTE_L
 */
#define SEGMENT_ATTRIBUTE_DB    (0x4000)
/**
 * Granularity flag.
 *
 * Determines the scailing of the segment limit field.
 *
 * When the granularity flag is clear, the segment limit is interpreted in byte units; When flag is
 * set, the segment limit is interpreted in 4KB units.
 *
 * This dose not affect the granularity of the base address; it is always byte granular.
 *
 * In IA-32e mode, this flag is always cleared because we don't use limit field of the descriptor.
 */
#define SEGMENT_ATTRIBUTE_G     (0x8000)

struct global_descriptor_table {
    struct segment_descriptor null;
    struct segment_descriptor kernel_code;
    struct segment_descriptor kernel_data;
    struct segment_descriptor user_null;
    struct segment_descriptor user_code;
    struct segment_descriptor user_data;
    struct task_state_segment_descriptor task_state;
} __attribute__((packed, aligned(0x1000)));

#endif
