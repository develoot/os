#ifndef _INTERRUPTS_CONTROL_INTERRUPTS
#define _INTERRUPTS_CONTROL_INTERRUPTS

#include <stdint.h>

void enable_interrupts(void);
void disable_interrupts(void);
uint64_t read_rflags(void);

#endif
