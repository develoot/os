#include <debug/assert.h>

#include "global_descriptor_table.h"
#include "global_descriptor_table_type.h"

/**
 * A static, constant global variable that represents the GDT.
 *
 * Base address of this structure should be aligned on an eight-byte boundary to yield the best
 * processor performance.
 */
__attribute__((aligned(0x1000)))
static const struct global_descriptor_table global_descriptor_table = {
    .null = {
        .limit = 0,
        .address0 = 0x0000,
        .address1 = 0x00,
        .attribute = 0x00,
        .address2 = 0x00
    },
    .kernel_code = {
        .limit = 0,
        .address0 = 0x0000,
        .address1 = 0x00,
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
        .attribute = 0x2098,
        .address2 = 0x00
    },
    .kernel_data = {
        .limit = 0,
        .address0 = 0x0000,
        .address1 = 0x00,
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
        .attribute = 0x0092,
        .address2 = 0x00
    },
    .user_null = {
        .limit = 0,
        .address0 = 0x0000,
        .address1 = 0x00,
        .attribute = 0x00,
        .address2 = 0x00
    },
    .user_code = {
        .limit =0,
        .address0 = 0,
        .address1 = 0,
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
        .attribute = 0x20F8,
        .address2 = 0x00
    },
    .user_data = {
        .limit = 0,
        .address0 = 0x0000,
        .address1 = 0x00,
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
        .attribute = 0x00F2,
        .address2 = 0x00
    }
};

void load_global_descriptor_table(void)
{
    assert(sizeof(struct global_descriptor_table) % 8 == 0, "GDT is not packed");
    assert(sizeof(struct global_descriptor_table_register) == 10, "GDTR entry is not packed");
    assert(sizeof(struct segment_descriptor) == 8, "Segment Descriptor is not packed");

    const struct global_descriptor_table_register register_entry = {
        .table_limit = sizeof(struct global_descriptor_table) - 1,
        .table_address = (uint64_t)&global_descriptor_table
    };

    /*
     * Segment Selector:
     *
     * bit [0:1]: Requested Privilege Level
     *  - Specifies the privilege level of the selector.
     *    The privilege level can range from 0 to 3, with 0 being the most privileged level.
     *
     * bit [2]: Table Indicator Flag
     *  - Specifies the descriptor to use.
     *    Clearing this flag selects the GDT, setting the flag selects the current LDT.
     *
     * bit [3:15]: Index
     *  - Selects one of descriptors in the GDT or LDT.
     */
    const uint16_t null_segment_selector = 0;
    const uint16_t kernel_code_segment_selector = (1 << 3) | (0 << 2) | 0x0;

    asm __volatile__ (
        "lgdt %0 \n\t"

        /*
         * Because ES, DS, and SS segment registers are not used in 64-bit mode, their fields in
         * segment descriptor registers are ignored.
         */
        "mov %1, %%ds \n\t"
        "mov %1, %%es \n\t"
        "mov %1, %%ss \n\t"

        /*
         * We will not use these segment registers.
         *
         * Set them null so that we can detect use of these segment registers using exception.
         */
        "mov %1, %%fs \n\t"
        "mov %1, %%gs \n\t"

        /*
         * TODO: Set the code segment register.
         *
         * For some reason, instruction below dose not work.
         *
         * UEFI set some value in the register. Just don't try to change the content for now.
         *
         * "pop %%rdi \n\t"
         * "push %2 \n\t"
         * "push %%rdi \n\t"
         * "retfq \n\t" // Far return.
         */

        :
        : "m"(register_entry), "r"(null_segment_selector), "r"(kernel_code_segment_selector)
        : "rdi"
    );
}
