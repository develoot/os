#include <kernel/console.h>

#include "control_register.h"
#include "controller.h"
#include "dummy_handlers.h"
#include "exception_vector_size.h"

void dummy_exception_handler(const uint8_t exception_number, const uint64_t error_code)
{
    console_print_format("Exception %u Error code %lu ", (uint32_t)exception_number, error_code);
    while (1);
}

void dummy_interrupt_handler(const uint8_t interrupt_number)
{
    interrupt_controller_notify_end(interrupt_number - EXCEPTION_VECTOR_SIZE);
}
