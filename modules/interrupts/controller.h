#ifndef _INTERRUPTS_CONTROLLER_H
#define _INTERRUPTS_CONTROLLER_H

#include <stdint.h>

void interrupt_controller_initialize(void);

void interrupt_controller_set_mask(uint16_t mask);

void interrupt_controller_notify_end(uint8_t interrupt_number);

#endif
