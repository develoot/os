#include <general/address.h>
#include <memory/segmentation/global_descriptor_table.h>
#include <memory/segmentation/segment_selector.h>

#include "interrupt_descriptor_table.h"
#include "programmable_interrupt_controller.h"
#include <asm/interrupts/interrupt_service_routines.h>

#include "initialize.h"

#define INTERRUPT_TABLE_SIZE (100)

__attribute__((aligned(0x08)))
static struct interrupt_gate_descriptor global_interrupt_descriptor_table[INTERRUPT_TABLE_SIZE];

static void register_interrupt_routine(struct interrupt_gate_descriptor *descriptor,
        void (*handler), uint16_t selector, uint16_t attribute)
{
    descriptor->offset0 = (address_t)handler & 0xFFFF;
    descriptor->offset1 = ((address_t)handler >> 16) & 0xFFFF;
    descriptor->offset2 = ((address_t)handler >> 32) & 0xFFFFFFFF;
    descriptor->segment_selector = selector;
    descriptor->attribute = attribute;
    descriptor->reserved = 0;
}

static void register_exception_routine(struct interrupt_gate_descriptor *descriptor,
        void (*handler))
{
    register_interrupt_routine(descriptor, handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));
}

int initialize_interrupts(void)
{
    struct interrupt_gate_descriptor *const table = global_interrupt_descriptor_table;

    register_exception_routine(&table[0],  divide_error_routine);
    register_exception_routine(&table[1],  debug_routine);
    register_exception_routine(&table[2],  non_maskable_interrupt_routine);
    register_exception_routine(&table[3],  break_point_routine);
    register_exception_routine(&table[4],  overflow_routine);
    register_exception_routine(&table[5],  bound_range_exceeded_routine);
    register_exception_routine(&table[6],  invalid_opcode_routine);
    register_exception_routine(&table[7],  device_not_available_routine);
    register_exception_routine(&table[8],  double_fault_routine);
    register_exception_routine(&table[9],  coprocessor_segment_overrun_routine);
    register_exception_routine(&table[10], invalid_opcode_routine);
    register_exception_routine(&table[11], segment_not_present_routine);
    register_exception_routine(&table[12], stack_full_routine);
    register_exception_routine(&table[13], general_protection_routine);
    register_exception_routine(&table[14], page_fault_routine);
    register_exception_routine(&table[15], x87_fpu_floating_point_error_routine);
    register_exception_routine(&table[16], alignment_check_routine);
    register_exception_routine(&table[17], machine_check_routine);
    register_exception_routine(&table[18], simd_floating_point_routine);
    register_exception_routine(&table[19], virtualization_routine);

    for (uint64_t i = 20; i < 32; i++) {
        register_exception_routine(&table[i], null_routine);
    }

    register_interrupt_routine(&table[32], timeout_routine,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[33], keyboard_routine,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    for (uint64_t i = 34; i < INTERRUPT_TABLE_SIZE; i++) {
        register_interrupt_routine(&table[i], null_routine,
                segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
                interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));
    }

    struct interrupt_descriptor_table_register_entry register_entry = {
        .table_limit = sizeof(global_interrupt_descriptor_table) - 1,
        .table_address = (address_t)global_interrupt_descriptor_table
    };

    asm __volatile__("lidt %0" : : "m"(register_entry));

    initialize_programmable_interrupt_controller();

    return 0;
}
