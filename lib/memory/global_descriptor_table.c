#include <debug/assert.h>

#include <asm/memory/do_load_global_descriptor_table.h>

#include "global_descriptor_table.h"
#include "global_descriptor_table_type.h"

#define SEGMENT_DESCRIPTOR(_limit, _address, _attribute) \
{                                                        \
    .limit     = (_limit),                               \
    .address0  = (_address),                             \
    .address1  = ((_address) >> 16) & 0xFF,              \
    .attribute = (_attribute),                           \
    .address2  = ((_address) >> 24) & 0xFF               \
}

/**
 * A static, constant global variable that represents the GDT.
 *
 * Base address of this structure should be aligned on an eight-byte boundary to yield the best
 * processor performance.
 */
__attribute__((aligned(0x1000)))
static const struct global_descriptor_table global_descriptor_table = {
    .null = SEGMENT_DESCRIPTOR(0, 0, 0x00),
    /*
     * SEGMENT_ATTRIBUTE_G = 0          // Have to be cleared in IA-32e descriptor.
     * SEGMENT_ATTRIBUTE_DB = 0         // Have to be 0 when L is set.
     * SEGMENT_ATTRIBUTE_L = 1          // Indicates that this is 64-bit code segment.
     * SEGMENT_ATTRIBUTE_AVL = 0        // We don't use this bit in this OS.
     * SEGMENT_ATTRIBUTE_LIMIT = 0b0000 // We don't use this field in IA-32e segment descriptor.
     * SEGMENT_ATTRIBUTE_P = 1          // Have to be set for all valid segments.
     * SEGMENT_ATTRIBUTE_DPL = 0b00     // This is kernel segment. Set the highest privilege.
     * SEGMENT_ATTRIBUTE_S = 1          // This is application segment.
     * SEGMENT_ATTRIBUTE_TYPE = 0b1000  // Executable, not conforming and not readable.
     */
    .kernel_code = SEGMENT_DESCRIPTOR(0, 0, 0x2098),
    /*
     * SEGMENT_ATTRIBUTE_G = 0          // Have to be cleared in IA-32e descriptor.
     * SEGMENT_ATTRIBUTE_DB = 0         // This flag is ignored in data segment descriptor.
     * SEGMENT_ATTRIBUTE_L = 0          // This flag should be unset in data segment descriptor.
     * SEGMENT_ATTRIBUTE_AVL = 0        // We don't use this bit in this OS.
     * SEGMENT_ATTRIBUTE_LIMIT = 0b0000 // We don't use this field in IA-32e segment descriptor.
     * SEGMENT_ATTRIBUTE_P = 1          // Have to be set for all valid segments.
     * SEGMENT_ATTRIBUTE_DPL = 0b00     // This is kernel segment. Set the highest privilege.
     * SEGMENT_ATTRIBUTE_S = 1          // This is application segment.
     * SEGMENT_ATTRIBUTE_TYPE = 0b0010  // Readable and writable.
     */
    .kernel_data = SEGMENT_DESCRIPTOR(0, 0, 0x0092),
    .user_null = SEGMENT_DESCRIPTOR(0, 0, 0x00),
    /*
     * SEGMENT_ATTRIBUTE_G = 0          // Have to be cleared in IA-32e descriptor.
     * SEGMENT_ATTRIBUTE_DB = 0         // Have to be 0 when L is set.
     * SEGMENT_ATTRIBUTE_L = 1          // Indicates that this is 64-bit code segment.
     * SEGMENT_ATTRIBUTE_AVL = 0        // We don't use this bit in this OS.
     * SEGMENT_ATTRIBUTE_LIMIT = 0b0000 // We don't use this field in IA-32e segment descriptor.
     * SEGMENT_ATTRIBUTE_P = 1          // Have to be set for all valid segments.
     * SEGMENT_ATTRIBUTE_DPL = 0b11     // This is user segment. Set the lowest privilege.
     * SEGMENT_ATTRIBUTE_S = 1          // This is application segment.
     * SEGMENT_ATTRIBUTE_TYPE = 0b1000  // Executable, not conforming and not readable.
     */
    .user_code = SEGMENT_DESCRIPTOR(0, 0, 0x20F8),
    /*
     * SEGMENT_ATTRIBUTE_G = 0          // Have to be cleared in IA-32e descriptor.
     * SEGMENT_ATTRIBUTE_DB = 0         // This flag is ignored in data segment descriptor.
     * SEGMENT_ATTRIBUTE_L = 0          // This flag should be unset in data segment descriptor.
     * SEGMENT_ATTRIBUTE_AVL = 0        // We don't use this bit in this OS.
     * SEGMENT_ATTRIBUTE_LIMIT = 0b0000 // We don't use this field in IA-32e segment descriptor.
     * SEGMENT_ATTRIBUTE_P = 1          // Have to be set for all valid segments.
     * SEGMENT_ATTRIBUTE_DPL = 0b11     // This is user segment. Set the lowest privilege.
     * SEGMENT_ATTRIBUTE_S = 1          // This is application segment.
     * SEGMENT_ATTRIBUTE_TYPE = 0b0010  // Readable and writable.
     */
    .user_data = SEGMENT_DESCRIPTOR(0, 0, 0x00F2),
};

void load_global_descriptor_table(void)
{
    assert(sizeof(struct global_descriptor_table) % 8 == 0, "GDT is not packed");
    assert(sizeof(struct global_descriptor_table_register) == 10, "GDTR entry is not packed");
    assert(sizeof(struct segment_descriptor) == 8, "Segment Descriptor is not packed");

    struct global_descriptor_table_register register_entry = {
        .table_limit = sizeof(struct global_descriptor_table) - 1,
        .table_address = (uint64_t)&global_descriptor_table
    };

    do_load_global_descriptor_table(&register_entry);
}
