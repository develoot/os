#ifndef _INTERRUPTS_CONTROL_INTERRUPTS
#define _INTERRUPTS_CONTROL_INTERRUPTS

#include <stdint.h>

#include <general/inline.h>

always_inline void enable_interrupts(void)
{
    asm __volatile__("sti");
}

always_inline void disable_interrupts(void)
{
    asm __volatile__("cli");
}

always_inline uint64_t read_rflags(void)
{
    uint64_t result = 0;

    asm __volatile__(
        "pushfq \n\t"
        "pop %0 \n\t"
        : "=m"(result)
    );

    return result;
}

#endif
