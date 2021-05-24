#include <cpu/port.h>
#include <general/address.h>
#include <memory/segmentation/global_descriptor_table.h>
#include <memory/segmentation/segment_selector.h>

#include "interrupt_descriptor_table.h"
#include "programmable_interrupt_controller.h"
#include <asm/interrupts/interrupt_service_routines.h>

#include "initialize.h"

#define register_interrupt_handler(Interrupt, Handler, Selector, Attribute) ({                      \
    global_interrupt_descriptor_table.Interrupt.offset0 = (address_t)(Handler) & 0xFFFF;            \
    global_interrupt_descriptor_table.Interrupt.offset1 = ((address_t)(Handler) >> 16) & 0xFFFF;    \
    global_interrupt_descriptor_table.Interrupt.offset2 = ((address_t)(Handler) >> 32) & 0xFFFFFFFF;\
    global_interrupt_descriptor_table.Interrupt.segment_selector = (Selector);                      \
    global_interrupt_descriptor_table.Interrupt.attribute = (Attribute);                            \
})

#define register_exception_handler(Exception, Handler) ({                                           \
    register_interrupt_handler(Exception, Handler,                                                  \
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),              \
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));   \
})

static struct interrupt_descriptor_table global_interrupt_descriptor_table;

int initialize_interrupts(void)
{
    register_exception_handler(divide_error,                 divide_error_routine);
    register_exception_handler(debug,                        debug_routine);
    register_exception_handler(non_maskable_interrupt,       non_maskable_interrupt_routine);
    register_exception_handler(break_point,                  break_point_routine);
    register_exception_handler(overflow,                     overflow_routine);
    register_exception_handler(bound_range_exceeded,         bound_range_exceeded_routine);
    register_exception_handler(invalid_opcode,               invalid_opcode_routine);
    register_exception_handler(device_not_available,         device_not_available_routine);
    register_exception_handler(double_fault,                 double_fault_routine);
    register_exception_handler(coprocessor_segment_overrun,  coprocessor_segment_overrun_routine);
    register_exception_handler(invalid_task_state_segment,   invalid_opcode_routine);
    register_exception_handler(segment_not_present,          segment_not_present_routine);
    register_exception_handler(stack_full,                   stack_full_routine);
    register_exception_handler(general_protection,           general_protection_routine);
    register_exception_handler(page_fault,                   page_fault_routine);
    register_exception_handler(x87_fpu_floating_point_error, x87_fpu_floating_point_error_routine);
    register_exception_handler(alignment_check,              alignment_check_routine);
    register_exception_handler(machine_check,                machine_check_routine);
    register_exception_handler(simd_floating_point,          simd_floating_point_routine);
    register_exception_handler(virtualization,               virtualization_routine);

    register_interrupt_handler(timeout, timeout_routine,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_interrupt_handler(keyboard, keyboard_routine,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    struct interrupt_descriptor_table_register_entry register_entry = {
        .table_limit = sizeof(global_interrupt_descriptor_table) - 1,
        .table_address = (address_t)&global_interrupt_descriptor_table
    };

    asm __volatile__("lidt %0" : : "m"(register_entry));

    initialize_programmable_interrupt_controller();

    return 0;
}
