#include <kernel/print.h>

#include "control_interrupts.h"
#include "exception_vector_size.h"
#include "programmable_interrupt_controller.h"

#include "dummy_interrupt_handler.h"

always_inline void _dummy_interrupt_handler(const uint8_t interrupt_number)
{
    print_format("Interrupt %u ", (uint32_t)interrupt_number);
    notify_end_of_interrupt(interrupt_number - EXCEPTION_VECTOR_SIZE);
}

void dummy_interrupt_handler(const uint8_t interrupt_number)
{
    if ((read_rflags() & REGISTER_RFLAGS_INTERRUPT) > 0) {
        disable_interrupts();
        _dummy_interrupt_handler(interrupt_number);
        enable_interrupts();
    } else {
        _dummy_interrupt_handler(interrupt_number);
    }
}
