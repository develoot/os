#include <kernel/print.h>

#include "control_register.h"
#include "controller.h"
#include "dummy_interrupt_handler.h"
#include "exception_vector_size.h"

void dummy_interrupt_handler(const uint8_t interrupt_number)
{
    interrupt_controller_notify_end(interrupt_number - EXCEPTION_VECTOR_SIZE);
}
