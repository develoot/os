#include <debug/assert.h>
#include <drivers/keyboard/manager.h>

#include "print.h"
#include "shell.h"

void shell_start(void)
{
    char input;
    int result;

    result = keyboard_manager_initialize();
    assert(result == 0, "Failed to activate keyboard");

    while (1) {
        result = keyboard_manager_get_input(&input);
        if (result == 0) {
            if (input != ' ' && input != '\n' && input != '\t' && (input < 'a' || 'z' < input)) {
                continue;
            }
            print_char(input);
        }
    }
}
