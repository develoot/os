#include <memory/segmentation/global_descriptor_table.h>
#include <memory/segmentation/segment_selector.h>

#include "exception_handlers.h"
#include "interrupt_descriptor_table.h"
#include "init.h"

#define register_exception_handler(Exception, Handler, Selector, Attribute)                        \
({                                                                                                 \
    global_interrupt_descriptor_table.Exception.offset0 = (uint16_t)((uint64_t)Handler & 0xFFFF);  \
    global_interrupt_descriptor_table.Exception.offset1                                            \
            = (uint16_t)(((uint64_t)Handler >> 16) & 0xFFFF);                                      \
    global_interrupt_descriptor_table.Exception.offset2                                            \
            = (uint32_t)(((uint64_t)Handler >> 32) & 0xFFFFFFF);                                   \
    global_interrupt_descriptor_table.Exception.segment_selector = Selector;                       \
    global_interrupt_descriptor_table.Exception.attribute = Attribute;                             \
})

static struct interrupt_descriptor_table global_interrupt_descriptor_table;

int init_interrupts(void)
{
    register_exception_handler(divide_error, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(debug, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(non_maskable_interrupt, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(break_point, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(overflow, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(bound_range_exceeded, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(invalid_opcode, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(device_not_available, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(double_fault, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(coprocessor_segment_overrun, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(invalid_task_state_segment, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(segment_not_present, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(stack_full, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(general_protection, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(page_fault, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(x87_fpu_floating_point_error, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(alignment_check, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(machine_check, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(simd_floating_point, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    register_exception_handler(virtualization, dummy_handler,
            segment_selector(GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_SEGMENT_INDEX, 0, 0),
            interrupt_gate_descriptor_attribute(1, INTERRUPT_GATE_DESCRIPTOR_TYPE_INTERRUPT, 0));

    return 0;
}
