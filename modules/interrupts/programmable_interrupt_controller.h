#ifndef _INTERRUPTS_PROGRAMMABLE_INTERRUPT_CONTROLLER_H
#define _INTERRUPTS_PROGRAMMABLE_INTERRUPT_CONTROLLER_H

#include <stdint.h>

void initialize_programmable_interrupt_controller(void);

void mask_interrupt(uint16_t mask);

#endif
