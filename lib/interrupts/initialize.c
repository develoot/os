#include <cpu/port.h>
#include <general/address.h>
#include <memory/segmentation/global_descriptor_table.h>
#include <memory/segmentation/segment_selector.h>

#include "exception_handlers.h"
#include "interrupt_descriptor_table.h"
#include "programmable_interrupt_controller.h"

#include "initialize.h"

#define register_interrupt_handler(Interrupt, Handler, Selector, Attribute) ({                      \
    global_interrupt_descriptor_table.Interrupt.offset0 = (address_t)(Handler) & 0xFFFF;            \
    global_interrupt_descriptor_table.Interrupt.offset1 = ((address_t)(Handler) >> 16) & 0xFFFF;    \
    global_interrupt_descriptor_table.Interrupt.offset2 = ((address_t)(Handler) >> 32) & 0xFFFFFFF; \
    global_interrupt_descriptor_table.Interrupt.segment_selector = (Selector);                      \
    global_interrupt_descriptor_table.Interrupt.attribute = (Attribute);                            \
})

#define register_exception_handler(Exception, Handler) ({                                           \
    register_interrupt_handler(Exception, Handler,                                                  \
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),              \
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));   \
})

static struct interrupt_descriptor_table global_interrupt_descriptor_table;

static void initialize_programmable_interrupt_controller(void)
{
    write_port(pic_master0, ICW1);
    write_port(pic_master1, ICW2MASTER);
    write_port(pic_master1, ICW3MASTER);
    write_port(pic_master1, ICW4);

    write_port(pic_slave0, ICW1);
    write_port(pic_slave1, ICW2SLAVE);
    write_port(pic_slave1, ICW3SLAVE);
    write_port(pic_slave1, ICW4);
}

static void mask_interrupt(uint16_t mask)
{
    // Check OCW1 command in the 8259A data sheet for detailed description.
    write_port(pic_master1, (uint8_t)mask);
    write_port(pic_slave1, (uint8_t)(mask >> 8));
}

int initialize_interrupts(void)
{
    register_exception_handler(divide_error,                 dummy_handler);
    register_exception_handler(debug,                        dummy_handler);
    register_exception_handler(non_maskable_interrupt,       dummy_handler);
    register_exception_handler(break_point,                  dummy_handler);
    register_exception_handler(overflow,                     dummy_handler);
    register_exception_handler(bound_range_exceeded,         dummy_handler);
    register_exception_handler(invalid_opcode,               dummy_handler);
    register_exception_handler(device_not_available,         dummy_handler);
    register_exception_handler(double_fault,                 dummy_handler);
    register_exception_handler(coprocessor_segment_overrun,  dummy_handler);
    register_exception_handler(invalid_task_state_segment,   dummy_handler);
    register_exception_handler(segment_not_present,          dummy_handler);
    register_exception_handler(stack_full,                   dummy_handler);
    register_exception_handler(general_protection,           dummy_handler);
    register_exception_handler(page_fault,                   dummy_handler);
    register_exception_handler(x87_fpu_floating_point_error, dummy_handler);
    register_exception_handler(alignment_check,              dummy_handler);
    register_exception_handler(machine_check,                dummy_handler);
    register_exception_handler(simd_floating_point,          dummy_handler);
    register_exception_handler(virtualization,               dummy_handler);

    initialize_programmable_interrupt_controller();

    struct interrupt_descriptor_table_register_entry register_entry = {
        .table_limit = sizeof(global_interrupt_descriptor_table) - 1,
        .table_address = (address_t)&global_interrupt_descriptor_table
    };

    asm __volatile__("lidt %0" : : "m"(register_entry));

    return 0;
}
