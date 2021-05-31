#include <kernel/print.h>

#include "control_interrupts.h"
#include "exception_vector_size.h"
#include "programmable_interrupt_controller.h"

#include "dummy_interrupt_handler.h"

void dummy_interrupt_handler(const uint8_t interrupt_number)
{
    print_format("Interrupt %u ", (uint32_t)interrupt_number);
    notify_end_of_interrupt(interrupt_number - EXCEPTION_VECTOR_SIZE);
}
