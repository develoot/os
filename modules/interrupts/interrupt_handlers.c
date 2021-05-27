#include <kernel/print.h>

#include "programmable_interrupt_controller.h"

#include "interrupt_handlers.h"

void dummy_interrupt_handler(uint8_t interrupt_number)
{
    print_format("Interrupt %u ", (uint32_t)interrupt_number);

    // The exception vector size is 32 in the IA-32e architecture.
    notify_end_of_interrupt(interrupt_number - 32);
}
