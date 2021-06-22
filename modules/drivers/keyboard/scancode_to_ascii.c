#include "manager.h"
#include "scancode_to_ascii.h"

struct scancode_to_ascii_table_entry {
    char normal_code;
    char combined_code;
};

static const struct scancode_to_ascii_table_entry conversion_table[] = {
    { ASCII_NONE, ASCII_NONE },
    { ASCII_ESC, ASCII_ESC },
    { '1', '!' },
    { '2', '@' },
    { '3', '#' },
    { '4', '$' },
    { '5', '%' },
    { '6', '^' },
    { '7', '&' },
    { '8', '*' },
    { '9', '(' },
    { '0', ')' },
    { '-', '_' },
    { '=', '+' },
    { ASCII_BACKSPACE, ASCII_BACKSPACE },
    { ASCII_TAB, ASCII_TAB },
    { 'q', 'Q' },
    { 'w', 'W' },
    { 'e', 'E' },
    { 'r', 'R' },
    { 't', 'T' },
    { 'y', 'Y' },
    { 'u', 'U' },
    { 'i', 'I' },
    { 'o', 'O' },
    { 'p', 'P' },
    { '[', '{' },
    { ']', '}' },
    { '\n', '\n' },
    { ASCII_CTRL, ASCII_CTRL },
    { 'a', 'A' },
    { 's', 'S' },
    { 'd', 'D' },
    { 'f', 'F' },
    { 'g', 'G' },
    { 'h', 'H' },
    { 'j', 'J' },
    { 'k', 'K' },
    { 'l', 'L' },
    { ';', ':' },
    { '\'', '\"' },
    { '`', '~' },
    { ASCII_LSHIFT, ASCII_LSHIFT },
    { '\\', '|' },
    { 'z', 'Z' },
    { 'x', 'X' },
    { 'c', 'C' },
    { 'v', 'V' },
    { 'b', 'B' },
    { 'n', 'N' },
    { 'm', 'M' },
    { ',', '<' },
    { '.', '>' },
    { '/', '?' },
    { ASCII_RSHIFT, ASCII_RSHIFT },
    { '*', '*' },
    { ASCII_LALT, ASCII_LALT },
    { ' ', ' ' },
    { ASCII_CAPSLOCK, ASCII_CAPSLOCK },
    { ASCII_F1, ASCII_F1 },
    { ASCII_F2, ASCII_F2 },
    { ASCII_F3, ASCII_F3 },
    { ASCII_F4, ASCII_F4 },
    { ASCII_F5, ASCII_F5 },
    { ASCII_F6, ASCII_F6 },
    { ASCII_F7, ASCII_F7 },
    { ASCII_F8, ASCII_F8 },
    { ASCII_F9, ASCII_F9 },
    { ASCII_F10, ASCII_F10 },
    { ASCII_NUMLOCK, ASCII_NUMLOCK },
    { ASCII_SCROLLLOCK , ASCII_SCROLLLOCK},
    { ASCII_HOME, '7' },
    { ASCII_UP, '8' },
    { ASCII_PAGEUP, '9' },
    { '-', '-' },
    { ASCII_LEFT, '4' },
    { ASCII_CENTER, '5' },
    { ASCII_RIGHT, '6' },
    { '+', '+' },
    { ASCII_END, '1' },
    { ASCII_DOWN, '2' },
    { ASCII_PAGEDOWN, '3' },
    { ASCII_INS, '0' },
    { ASCII_DEL, '.' },
    { ASCII_NONE, ASCII_NONE },
    { ASCII_NONE, ASCII_NONE },
    { ASCII_NONE, ASCII_NONE },
    { ASCII_F11, ASCII_F11 },
    { ASCII_F12, ASCII_F12 }
};

static inline bool is_alphabet(uint8_t scancode)
{
    return (conversion_table[scancode].normal_code >= 'a')
        && (conversion_table[scancode].normal_code <= 'z');
}

static inline bool is_number_or_symbol(uint8_t scancode)
{
    return (scancode >= 2)
        && (scancode <= 53)
        && (!is_alphabet(scancode));
}

static inline bool is_numbpad(uint8_t scancode)
{
    return (scancode >= 71)
        && (scancode <= 83);
}

static inline bool should_use_combined_key(uint8_t scancode)
{
    uint8_t down_scancode = scancode & 0x7F;

    if (is_alphabet(down_scancode)) {
        return keyboard_manager_is_shift_down() != keyboard_manager_is_capslock_on();
    }

    if (is_number_or_symbol(down_scancode)) {
        return keyboard_manager_is_shift_down();
    }

    if (is_numbpad(down_scancode)) {
        return keyboard_manager_is_numlock_on();
    }

    return 0;
}

char scancode_to_ascii_convert(uint8_t scancode)
{
    return should_use_combined_key(scancode)
        ? conversion_table[scancode].combined_code
        : conversion_table[scancode].normal_code;
}
