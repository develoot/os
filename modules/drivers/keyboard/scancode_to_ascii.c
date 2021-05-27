#include "keyboard_manager.h"
#include "scancode_to_ascii.h"

struct scancode_to_ascii_table_entry {
    char normal_code;
    char combined_code;
};

static const struct scancode_to_ascii_table_entry conversion_table[] = {
    { KEY_NONE, KEY_NONE },
    { KEY_ESC, KEY_ESC },
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
    { KEY_BACKSPACE, KEY_BACKSPACE },
    { KEY_TAB, KEY_TAB },
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
    { KEY_CTRL, KEY_CTRL },
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
    { KEY_LSHIFT, KEY_LSHIFT },
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
    { KEY_RSHIFT, KEY_RSHIFT },
    { '*', '*' },
    { KEY_LALT, KEY_LALT },
    { ' ', ' ' },
    { KEY_CAPSLOCK, KEY_CAPSLOCK },
    { KEY_F1, KEY_F1 },
    { KEY_F2, KEY_F2 },
    { KEY_F3, KEY_F3 },
    { KEY_F4, KEY_F4 },
    { KEY_F5, KEY_F5 },
    { KEY_F6, KEY_F6 },
    { KEY_F7, KEY_F7 },
    { KEY_F8, KEY_F8 },
    { KEY_F9, KEY_F9 },
    { KEY_F10, KEY_F10 },
    { KEY_NUMLOCK, KEY_NUMLOCK },
    { KEY_SCROLLLOCK , KEY_SCROLLLOCK},
    { KEY_HOME, '7' },
    { KEY_UP, '8' },
    { KEY_PAGEUP, '9' },
    { '-', '-' },
    { KEY_LEFT, '4' },
    { KEY_CENTER, '5' },
    { KEY_RIGHT, '6' },
    { '+', '+' },
    { KEY_END, '1' },
    { KEY_DOWN, '2' },
    { KEY_PAGEDOWN, '3' },
    { KEY_INS, '0' },
    { KEY_DEL, '.' },
    { KEY_NONE, KEY_NONE },
    { KEY_NONE, KEY_NONE },
    { KEY_NONE, KEY_NONE },
    { KEY_F11, KEY_F11 },
    { KEY_F12, KEY_F12 }
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
        return is_shift_down() != is_capslock_on();
    }

    if (is_number_or_symbol(down_scancode)) {
        return is_shift_down();
    }

    if (is_numbpad(down_scancode)) {
        return is_numlock_on();
    }

    return 0;
}

char convert_scancode_to_ascii(uint8_t scancode)
{
    return should_use_combined_key(scancode)
        ? conversion_table[scancode].combined_code
        : conversion_table[scancode].normal_code;
}
