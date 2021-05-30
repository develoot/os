#ifndef _DRIVERS_KEYBOARD_KEYBOARD_INTERRUPT_HANDLER_H
#define _DRIVERS_KEYBOARD_KEYBOARD_INTERRUPT_HANDLER_H

#include <stdint.h>

typedef uint8_t scancode_t;

void initialize_keyboard_queue(void);

void keyboard_interrupt_handler(const uint8_t interrupt_number);

scancode_t get_scancode(void);

#endif
