#include <kernel/print.h>

#include "exception_handlers.h"

void dummy_exception_handler(uint8_t exception_number, uint64_t error_code)
{
    print_format("Exception %u Error code %lu ", (uint32_t)exception_number, error_code);
    while (1) {}
}
