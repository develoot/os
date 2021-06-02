#include <asm/interrupts/service_routines.h>
#include <debug/assert.h>
#include <general/address.h>
#include <memory/global_descriptor_table.h>
#include <memory/segment_selector.h>

#include "descriptor_table.h"
#include "controller.h"
#include "initialize.h"

#define INTERRUPT_TABLE_SIZE (256)

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
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));
}

int interrupts_initialize(void)
{
    assert(sizeof(struct interrupt_gate_descriptor) != 128, "Size of IDT descriptor is invalid");

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
    register_exception_routine(&table[15], null_exception_routine);
    register_exception_routine(&table[16], x87_fpu_floating_point_error_routine);
    register_exception_routine(&table[17], alignment_check_routine);
    register_exception_routine(&table[18], machine_check_routine);
    register_exception_routine(&table[19], simd_floating_point_routine);
    register_exception_routine(&table[20], virtualization_routine);

    for (uint64_t i = 20; i < 32; i++) {
        register_exception_routine(&table[i], null_exception_routine);
    }

    register_interrupt_routine(&table[32], timeout_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[33], keyboard_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[34], slave_pic_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[35], serial_port2_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[36], serial_port1_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[37], parallel_port2_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[38], floppy_controller_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[39], parallel_port1_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[40], real_time_check_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    for (uint64_t i = 41; i < 44; i++) {
        register_interrupt_routine(&table[i], null_interrupt_routine,
                segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
                interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));
    }

    register_interrupt_routine(&table[44], mouse_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[45], coprocessor_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[46], hdd1_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_routine(&table[47], hdd2_routine,
            segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    for (uint64_t i = 48; i < INTERRUPT_TABLE_SIZE; i++) {
        register_interrupt_routine(&table[i], null_interrupt_routine,
                segment_selector(0, 0, GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX),
                interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));
    }

    struct interrupt_descriptor_table_register_entry register_entry = {
        .table_limit = sizeof(global_interrupt_descriptor_table) - 1,
        .table_address = (address_t)global_interrupt_descriptor_table
    };

    asm __volatile__("lidt %0" : : "m"(register_entry));

    interrupt_controller_initialize();

    asm __volatile__("sti");

    return 0;
}
