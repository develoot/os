#include <debug/assert.h>
#include <drivers/keyboard/manager.h>

#include "print.h"
#include "shell.h"

void shell_start(void)
{
    char input;
    int result;

    keyboard_manager_initialize();

    result = keyboard_manager_activate_keyboard();
    assert(result == 0, "Failed to activate keyboard");

    while (1) {
        result = keyboard_manager_get_input(&input);
        if (result == 0) {
            print_char(input);
        }
    }
}
