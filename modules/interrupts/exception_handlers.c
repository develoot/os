#include <kernel/print.h>

#include "exception_handlers.h"

void dummy_exception_handler(uint8_t exception_number)
{
    print_format("Exception %u ", (uint32_t)exception_number);
    while (1) {}
}
