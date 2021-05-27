#ifndef _DRIVERS_KEYBOARD_KEYBOARD_MANAGER_H
#define _DRIVERS_KEYBOARD_KEYBOARD_MANAGER_H

#include <stdbool.h>

struct keyboard_manager_data {
    bool is_capslock_on;
    bool is_numlock_on;
    bool is_scroll_lock_on;
    bool is_shift_down;
};

void initialize_keyboard_manager(void);

int activate_keyboard(void);

void enable_a20_gate(void);

void reset_processor(void);

int get_keyboard_input(char *const out);

bool is_capslock_on(void);

bool is_numlock_on(void);

bool is_scroll_lock_on(void);

bool is_shift_down(void);

#endif
