#include <stddef.h>

#include <asm/memory/load_global_descriptor_table.h>
#include <debug/assert.h>
#include <memory/page_frame_allocator.h>

#include "global_descriptor_table.h"
#include "task_state_segment.h"
#include "initialize.h"

/**
 * A static, constant global variable that represents the TSS.
 *
 * Base address of this structure should be aligned on an eight-byte boundary to yield the best
 * processor performance.
 */
__attribute__((aligned(0x1000)))
static struct task_state_segment global_task_state_segment;

#define APPLICATION_SEGMENT_DESCRIPTOR(Limit, Address, Attribute)   \
{                                                                   \
    .limit     = (Limit),                                           \
    .address0  = (Address),                                         \
    .address1  = ((Address) >> 16) & 0xFF,                          \
    .attribute = (Attribute),                                       \
    .address2  = ((Address) >> 24) & 0xFF                           \
}

/**
 * A static, constant global variable that represents the GDT.
 *
 * Base address of this structure should be aligned on an eight-byte boundary to yield the best
 * processor performance.
 */
__attribute__((aligned(0x1000)))
static struct global_descriptor_table global_descriptor_table = {
    .null = APPLICATION_SEGMENT_DESCRIPTOR(0, 0, 0x00),
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
    .kernel_code = APPLICATION_SEGMENT_DESCRIPTOR(0, 0, 0x2098),
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
    .kernel_data = APPLICATION_SEGMENT_DESCRIPTOR(0, 0, 0x0092),
    .user_null = APPLICATION_SEGMENT_DESCRIPTOR(0, 0, 0x00),
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
    .user_code = APPLICATION_SEGMENT_DESCRIPTOR(0, 0, 0x20F8),
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
    .user_data = APPLICATION_SEGMENT_DESCRIPTOR(0, 0, 0x00F2),
};

static int initialize_global_task_state_segment(void)
{
    global_task_state_segment.rsp[0] = 0x00;
    global_task_state_segment.rsp[1] = 0x00;
    global_task_state_segment.rsp[2] = 0x00;

    global_task_state_segment.interrupt_stack_table[0] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[1] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[2] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[3] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[4] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[5] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[6] = 0x00; // TODO:

    // This effectively disables the bitmap field of the TSS.
    global_task_state_segment.io_bitmap_base = sizeof(global_task_state_segment) + 1;

    return 0;
}

void initialize_segmentation(void)
{
    assert(sizeof(struct global_descriptor_table) % 8 == 0, "GDT is not packed");
    assert(sizeof(struct global_descriptor_table_register_entry) == 10, "GDTR entry is not packed");
    assert(sizeof(struct application_segment_descriptor) == 8, "Segment Descriptor is not packed");
    assert(sizeof(struct task_state_segment) % 8 == 0, "TSS is not packed");

    struct global_descriptor_table_register_entry register_entry = {
        .table_limit = sizeof(global_descriptor_table) - 1,
        .table_address = (uint64_t)&global_descriptor_table
    };

    initialize_global_task_state_segment();

    /*
     * We can't assign the address of the TSS into member of GDT because address of the
     * `global_task_state_segment` is not a compile-time constant.
     */

    const uint64_t task_state_segment_address = (uint64_t)&global_task_state_segment;

    global_descriptor_table.task_state.limit     = sizeof(global_task_state_segment);
    global_descriptor_table.task_state.address0  = task_state_segment_address;
    global_descriptor_table.task_state.address1  = (task_state_segment_address >> 16) & 0xFF;
    /*
     * SEGMENT_ATTRIBUTE_G = 0          // Have to be cleared in IA-32e descriptor.
     * SEGMENT_ATTRIBUTE_DB = 0         // This flag is not used in TSS descriptor.
     * SEGMENT_ATTRIBUTE_L = 0          // This flag should be unset in non-code segment descriptor.
     * SEGMENT_ATTRIBUTE_AVL = 0        // We don't use this bit in this OS.
     * SEGMENT_ATTRIBUTE_LIMIT = 0b0000 // We don't use this field in IA-32e segment descriptor.
     * SEGMENT_ATTRIBUTE_P = 1          // Have to be set for all valid segments.
     * SEGMENT_ATTRIBUTE_DPL = 0b00     // This is kernel segment. Set the highest privilege.
     * SEGMENT_ATTRIBUTE_S = 0          // This is a system segment.
     * SEGMENT_ATTRIBUTE_TYPE = 0b1001  // This is 64-bit Availabe TSS.
     */
    global_descriptor_table.task_state.attribute = 0x0089;
    global_descriptor_table.task_state.address2  = (task_state_segment_address >> 24) & 0xFF;
    global_descriptor_table.task_state.address3  = (task_state_segment_address >> 32) & 0xFFFFFFFF;
    global_descriptor_table.task_state.reserved  = 0;

    const uint16_t task_state_segment_offset = offsetof(struct global_descriptor_table, task_state);

    load_global_descriptor_table(&register_entry);
    asm __volatile__("ltr %0" // Load the TSS descriptor's offset from the base of the GDT.
        :
        : "m"(task_state_segment_offset)
        :
    );
}
