#include <kernel/print.h>

#include "dummy_exception_handler.h"

void dummy_exception_handler(const uint8_t exception_number, const uint64_t error_code)
{
    print_format("Exception %u Error code %lu ", (uint32_t)exception_number, error_code);
    while (1) {}
}
