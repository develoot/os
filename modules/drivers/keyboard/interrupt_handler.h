#ifndef _DRIVERS_KEYBOARD_INTERRUPT_HANDLER_H
#define _DRIVERS_KEYBOARD_INTERRUPT_HANDLER_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t scancode_t;

void keyboard_interrupt_handler_initialize(void);

void keyboard_interrupt_handler(const uint8_t interrupt_number);

bool keyboard_interrupt_handler_is_queue_empty(void);

scancode_t keyboard_interrupt_handler_get_scancode(void);

#endif
