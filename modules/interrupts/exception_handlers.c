#include <kernel/print.h>

#include "exception_handlers.h"

void dummy_exception_handler(void)
{
    print_string("Dummy exception handler called. ");

    while (1) {}
}
