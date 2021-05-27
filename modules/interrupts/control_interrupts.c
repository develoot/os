#include "control_interrupts.h"

void enable_interrupts(void)
{
    asm __volatile__("sti");
}

void disable_interrupts(void)
{
    asm __volatile__("cli");
}

uint64_t read_rflags(void)
{
    uint64_t result = 0;

    asm __volatile__(
        "pushfq \n\t"
        "pop %0 \n\t"
        : "=m"(result)
    );

    return result;
}
