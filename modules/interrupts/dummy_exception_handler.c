#include <kernel/print.h>

#include "control_interrupts.h"

#include "dummy_exception_handler.h"

always_inline void _dummy_exception_handler(const uint8_t exception_number, const uint64_t error_code)
{
    print_format("Exception %u Error code %lu ", (uint32_t)exception_number, error_code);
}

void dummy_exception_handler(const uint8_t exception_number, const uint64_t error_code)
{
    if ((read_rflags() & REGISTER_RFLAGS_INTERRUPT) > 0) {
        disable_interrupts();
        _dummy_exception_handler(exception_number, error_code);
        enable_interrupts();
    } else {
        _dummy_exception_handler(exception_number, error_code);
    }
}
