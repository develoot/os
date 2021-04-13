#include <debug/assert.h>
#include <drivers/keyboard/keyboard_manager.h>

#include "print.h"
#include "shell.h"

void start_shell(void)
{
    char input;
    int result;

    init_keyboard_manager();

    result = activate_keyboard();
    assert(result == 0, "Failed to activate keyboard");

    while (1) {
        result = get_keyboard_input(&input);
        if (result == 0) {
            print_char(input);
        }
    }
}
