#ifndef _INTERRUPTS_CONTROL_REGISTER_H
#define _INTERRUPTS_CONTROL_REGISTER_H

#include <stdint.h>

#define REGISTER_RFLAGS_INTERRUPT (0x0200)

static inline void interrupts_enable(void)
{
    asm __volatile__("sti");
}

static inline void interrupts_disable(void)
{
    asm __volatile__("cli");
}

static inline uint16_t get_rflags(void)
{
    uint16_t result = 0;

    asm __volatile__(
        "pushfq \n\t"
        "pop %0 \n\t"
        : "=m"(result)
    );

    return result;
}

#endif
