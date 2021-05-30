#ifndef _INTERRUPTS_CONTROL_INTERRUPTS
#define _INTERRUPTS_CONTROL_INTERRUPTS

#include <stdint.h>

#include <general/inline.h>

#define REGISTER_RFLAGS_INTERRUPT (0x0200)

always_inline void enable_interrupts(void)
{
    asm __volatile__("sti");
}

always_inline void disable_interrupts(void)
{
    asm __volatile__("cli");
}

uint16_t read_rflags(void);

#endif
