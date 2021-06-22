#ifndef _DRIVERS_KEYBOARD_MANAGER_H
#define _DRIVERS_KEYBOARD_MANAGER_H

#include <stdbool.h>

#include "ascii.h"

struct keyboard_manager_data {
    bool is_capslock_on;
    bool is_numlock_on;
    bool is_scroll_lock_on;
    bool is_shift_down;
};

int keyboard_manager_initialize(void);

void keyboard_manager_enable_a20(void);

void keyboard_manager_reset_processor(void);

int keyboard_manager_get_input(char *const out);

bool keyboard_manager_is_buffer_empty(void);

bool keyboard_manager_is_capslock_on(void);

bool keyboard_manager_is_numlock_on(void);

bool keyboard_manager_is_scroll_lock_on(void);

bool keyboard_manager_is_shift_down(void);

#endif
