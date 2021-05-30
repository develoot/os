#include <kernel/print.h>

#include "control_interrupts.h"
#include "dummy_exception_handler.h"

void dummy_exception_handler(const uint8_t exception_number, const uint64_t error_code)
{
    disable_interrupts();

    print_format("Exception %u Error code %lu ", (uint32_t)exception_number, error_code);
    while (1) {}

    enable_interrupts();
}
