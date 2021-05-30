#include <kernel/print.h>

#include "control_interrupts.h"
#include "dummy_exception_handler.h"

void dummy_exception_handler(const uint8_t exception_number, const uint64_t error_code)
{
    if ((read_rflags() & REGISTER_RFLAGS_INTERRUPT) > 0) {
        disable_interrupts();
        print_format("Exception %u Error code %lu ", (uint32_t)exception_number, error_code);
        enable_interrupts();
    } else {
        print_format("Exception %u Error code %lu ", (uint32_t)exception_number, error_code);
    }
}
