#include <kernel/print.h>

#include <interrupts/control_interrupts.h>
#include <interrupts/programmable_interrupt_controller.h>

#include "dummy_interrupt_handler.h"

void dummy_interrupt_handler(const uint8_t interrupt_number)
{
    disable_interrupts();

    print_format("Interrupt %u ", (uint32_t)interrupt_number);

    // The exception vector size is 32 in the IA-32e architecture.
    notify_end_of_interrupt(interrupt_number - 32);

    enable_interrupts();
}
