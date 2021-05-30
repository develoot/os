#include "control_interrupts.h"

uint16_t read_rflags(void)
{
    uint16_t result = 0;

    asm __volatile__(
        "pushfq \n\t"
        "pop %0 \n\t"
        : "=m"(result)
    );

    return result;
}
