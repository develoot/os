#ifndef _INTERRUPTS_PROGRAMMABLE_INTERRUPT_CONTROLLER_H
#define _INTERRUPTS_PROGRAMMABLE_INTERRUPT_CONTROLLER_H

#include <stdint.h>

void initialize_programmable_interrupt_controller(void);

void set_interrupt_mask(uint16_t mask);

void notify_end_of_interrupt(uint8_t interrupt_number);

#endif
