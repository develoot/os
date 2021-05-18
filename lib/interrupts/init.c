#include "interrupt_descriptor_table.h"
#include "init.h"

#define INTERRUPT_GATE_DESCRIPTOR(_offset, _selector, _attribute)                           \
{                                                                                           \
    .offset0          = (uint16_t)(_offset),                                                \
    .segment_selector = (uint16_t)(_selector),                                              \
    .attribute        = (uint16_t)(_attribute),                                             \
    .offset1          = (uint16_t)((_offset) >> 16) & 0xFFFF,                               \
    .offset2          = (uint32_t)((_offset) >> 32) & 0xFFFFFFFF,                           \
    .reserved         = 0                                                                   \
}

static const struct interrupt_descriptor_table global_interrupt_descriptor_table = {
    .divide_error = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0, 0, 0),
    .debug = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .non_maskable_interrupt = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .break_point = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .overflow = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .bound_range_exceeded = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .invalid_opcode = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .device_not_available = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .double_fault = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .coprocessor_segment_overrun = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .invalid_task_state_segment = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .segment_not_present = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .stack_full = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .general_protection = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .page_fault = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .x87_fpu_floating_point_error = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .alignment_check = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .machine_check = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .simd_floating_point = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
    .virtualization = INTERRUPT_GATE_DESCRIPTOR((uint64_t)0,0,0),
};

int init_interrupts(void)
{
    return 0;
}
