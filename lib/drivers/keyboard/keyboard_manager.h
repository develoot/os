#ifndef _DRIVERS_KEYBOARD_KEYBOARD_MANAGER_H
#define _DRIVERS_KEYBOARD_KEYBOARD_MANAGER_H

#include <general/boolean.h>

struct keyboard_manager_data {
    enum boolean is_capslock_on;
    enum boolean is_numlock_on;
    enum boolean is_scroll_lock_on;
    enum boolean is_shift_down;
};

void init_keyboard_manager(void);
int activate_keyboard(void);
void enable_a20_gate(void);
void reset_processor(void);
int get_keyboard_input(char *out);

#endif
