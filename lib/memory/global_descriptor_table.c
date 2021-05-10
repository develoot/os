#include <debug/assert.h>

#include <asm/memory/do_load_global_descriptor_table.h>

#include "global_descriptor_table.h"
#include "global_descriptor_table_type.h"
#include "page_frame_allocator.h"

#define SEGMENT_DESCRIPTOR(_limit, _address, _attribute) \
{                                                        \
    .limit     = (_limit),                               \
    .address0  = (_address),                             \
    .address1  = ((_address) >> 16) & 0xFF,              \
    .attribute = (_attribute),                           \
    .address2  = ((_address) >> 24) & 0xFF               \
}

/**
 * A static, constant global variable that represents the TSS.
 *
 * Base address of this structure should be aligned on an eight-byte boundary to yield the best
 * processor performance.
 */
__attribute__((aligned(0x1000)))
static struct task_state_segment global_task_state_segment;

/**
 * A static, constant global variable that represents the GDT.
 *
 * Base address of this structure should be aligned on an eight-byte boundary to yield the best
 * processor performance.
 */
__attribute__((aligned(0x1000)))
static struct global_descriptor_table global_descriptor_table = {
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

static int init_global_task_state_segment(void)
{
    page_frame_t ring0_stack = request_page_frame(512);
    if (ring0_stack == PAGE_FRAME_NULL) {
        return 1;
    }
    page_frame_t ring2_stack = request_page_frame(512);
    if (ring2_stack == PAGE_FRAME_NULL) {
        free_page_frame(ring0_stack); // TODO: Free multiple page frames.
        return 1;
    }

    global_task_state_segment.rsp0 = (uint64_t)ring0_stack;
    global_task_state_segment.rsp1 = (uint64_t)PAGE_FRAME_NULL; // We don't use ring1 privilege.
    global_task_state_segment.rsp2 = (uint64_t)ring2_stack;

    global_task_state_segment.interrupt_stack_table[0] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[1] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[2] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[3] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[4] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[5] = 0x00; // TODO:
    global_task_state_segment.interrupt_stack_table[6] = 0x00; // TODO:

    global_task_state_segment.io_bitmap_base = 0x00; // TODO:

    return 0;
}

void load_global_descriptor_table(void)
{
    assert(sizeof(struct global_descriptor_table) % 8 == 0, "GDT is not packed");
    assert(sizeof(struct global_descriptor_table_register) == 10, "GDTR entry is not packed");
    assert(sizeof(struct segment_descriptor) == 8, "Segment Descriptor is not packed");
    assert(sizeof(struct task_state_segment) % 8 == 0, "TSS is not packed");

    struct global_descriptor_table_register register_entry = {
        .table_limit = sizeof(struct global_descriptor_table) - 1,
        .table_address = (uint64_t)&global_descriptor_table
    };

    init_global_task_state_segment();

    /*
     * We need to load the TSS address into GDT here because address of the
     * `global_task_state_segment` is not a compile-time constant.
     */

    const uint64_t task_state_segment_address = (uint64_t)&global_task_state_segment;

    global_descriptor_table.task_state.limit     = 0;
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

    do_load_global_descriptor_table(&register_entry);

    // TODO: Load TSS via `ldt` instruction.
}
