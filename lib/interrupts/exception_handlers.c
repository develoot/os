#include <kernel/print.h>

#include "exception_handlers.h"

void dummy_handler(void)
{
    print_string("Dummy Handler Called. ");
    while (1) {}
}
