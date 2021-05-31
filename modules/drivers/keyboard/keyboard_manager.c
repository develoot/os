#include <cpu/port.h>

#include "scancode_to_ascii.h"
#include "keyboard_manager.h"

#define KEYBOARD_STATUS_OUTB (0x01) // Output buffer state. (0:Empty, 1:Full)
#define KEYBOARD_STATUS_INPB (0x02) // Input buffer state.
#define KEYBOARD_STATUS_SYSF (0x04) // System Flag, (0:Unavailage, 1:Available)
#define KEYBOARD_STATUS_CD   (0x08) // Indicate type of the last byte sent. (0:Command, 1:Data)
#define KEYBOARD_STATUS_KEYL (0x10) // Keyboard Lock Status.
#define KEYBOARD_STATUS_AUXB (0x20) // Output has auxiliary device. (e.g, mouse)
#define KEYBOARD_STATUS_TIM  (0x40) // General Time-Out.
#define KEYBOARD_STATUS_PARE (0x80) // Parity Error.

#define KEYBOARD_CMD_ACTIVATE_CONTROLLER (0xAE)
#define KEYBOARD_CMD_ACTIVATE_KEYBOARD   (0xF4)
#define KEYBOARD_CMD_ACK                 (0xFA)
#define KEYBOARD_CMD_CHANGE_LED          (0xED)
#define KEYBOARD_CMD_READ_CONTROLLER_OUT (0xD0)
#define KEYBOARD_CMD_SET_OUTPUT_PORT     (0xD1)

#define KEYBOARD_SCANCODE_LSHIFT_DOWN     (0x2A)
#define KEYBOARD_SCANCODE_LSHIFT_UP       (0xAA)
#define KEYBOARD_SCANCODE_RSHIFT_DOWN     (0x36)
#define KEYBOARD_SCANCODE_RSHIFT_UP       (0xB6)
#define KEYBOARD_SCANCODE_CAPSLOCK_DOWN   (0x3A)
#define KEYBOARD_SCANCODE_CAPSLOCK_UP     (0xBA)
#define KEYBOARD_SCANCODE_NUMLOCK_DOWN    (0x45)
#define KEYBOARD_SCANCODE_NUMLOCK_UP      (0xC5)
#define KEYBOARD_SCANCODE_SCROLLLOCK_DOWN (0x46)
#define KEYBOARD_SCANCODE_SCROLLLOCK_UP   (0xC6)

#define MAX_TRY (0xFFFFFFFF)

static struct keyboard_manager_data global_keyboard_manager_data;

static inline bool is_output_buffer_full(void)
{
    return (port_read(keyboard1) & KEYBOARD_STATUS_OUTB) > 0;
}

static inline bool is_input_buffer_full(void)
{
    return (port_read(keyboard1) & KEYBOARD_STATUS_INPB) > 0;
}

static uint8_t get_scancode(void)
{
    uint64_t try = 0;

    while (is_output_buffer_full() == false && try++ < MAX_TRY) {}

    return port_read(keyboard0);
}

static int change_keyboard_led(bool is_capslock_on, bool is_numlock_on, bool is_scroll_lock_on)
{
    uint64_t try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY) {}

    port_write(keyboard0, KEYBOARD_CMD_CHANGE_LED);

    // Wait until keyboard processes CHANGE_LED command.
    try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY) {}

    for (uint64_t i = 0; i < 100; ++i) {
        try = 0;
        while (is_output_buffer_full() == false && try++ < MAX_TRY) {}

        if (port_read(keyboard0) == KEYBOARD_CMD_ACK) {
            goto SUCCESS_FINDING_ACK;
        }
    }

    return 1;

SUCCESS_FINDING_ACK:
    port_write(keyboard0, ((is_capslock_on << 2) | (is_numlock_on << 1) | (is_scroll_lock_on)));

    try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY) {}

    for (uint64_t i = 0; i < 100; ++i) {
        try = 0;
        while (is_output_buffer_full() == false && try++ < MAX_TRY) {}

        if (port_read(keyboard0) == KEYBOARD_CMD_ACK) {
            return 0;
        }
    }

    return 1;
}

static void update_global_keyboard_manager_state(uint8_t scancode)
{
    if ((scancode == KEYBOARD_SCANCODE_LSHIFT_DOWN)
            || (scancode == KEYBOARD_SCANCODE_RSHIFT_DOWN)) {
        global_keyboard_manager_data.is_shift_down = true;
        return;
    }

    if ((scancode == KEYBOARD_SCANCODE_LSHIFT_UP)
            || (scancode == KEYBOARD_SCANCODE_RSHIFT_UP)) {
        global_keyboard_manager_data.is_shift_down = false;
        return;
    }

    if (scancode == KEYBOARD_SCANCODE_CAPSLOCK_DOWN) {
        global_keyboard_manager_data.is_capslock_on = !global_keyboard_manager_data.is_capslock_on;
        goto CHANGE_LED_STATUS;
    }

    if (scancode == KEYBOARD_SCANCODE_NUMLOCK_DOWN) {
        global_keyboard_manager_data.is_numlock_on = !global_keyboard_manager_data.is_numlock_on;
        goto CHANGE_LED_STATUS;
    }

    if (scancode == KEYBOARD_SCANCODE_NUMLOCK_DOWN) {
        global_keyboard_manager_data.is_scroll_lock_on =
            !global_keyboard_manager_data.is_scroll_lock_on;
        goto CHANGE_LED_STATUS;
    }

CHANGE_LED_STATUS:
    change_keyboard_led(global_keyboard_manager_data.is_capslock_on,
            global_keyboard_manager_data.is_numlock_on,
            global_keyboard_manager_data.is_scroll_lock_on);
}

void initialize_keyboard_manager(void)
{
    global_keyboard_manager_data.is_capslock_on    = false;
    global_keyboard_manager_data.is_numlock_on     = false;
    global_keyboard_manager_data.is_scroll_lock_on = false;
    global_keyboard_manager_data.is_shift_down     = false;
}

int activate_keyboard(void)
{
    port_write(keyboard1, KEYBOARD_CMD_ACTIVATE_CONTROLLER);

    // Wait until keyboard processes input so that input buffer is empty.
    // TODO: This implementation is dumb.
    uint64_t try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY) {}

    port_write(keyboard0, KEYBOARD_CMD_ACTIVATE_KEYBOARD);

    // Read 100 times from output buffer to find ACK message.
    for (uint64_t i = 0; i < 100; ++i) {
        try = 0;
        while (is_output_buffer_full() == false && try++ < MAX_TRY) {}

        if (port_read(keyboard0) == KEYBOARD_CMD_ACK) {
            return 0;
        }
    }

    return 1;
}

void enable_a20_gate(void)
{
    port_write(keyboard1, KEYBOARD_CMD_READ_CONTROLLER_OUT);

    uint64_t try = 0;
    while (is_output_buffer_full() == false && try++ < MAX_TRY) {}

    uint8_t data = port_read(keyboard0);
    data |= 0x01; // Set "Enable A20" bit.

    try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY) {}

    port_write(keyboard1, KEYBOARD_CMD_SET_OUTPUT_PORT);
    port_write(keyboard0, data);
}

void reset_processor(void)
{
    uint64_t try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY);

    port_write(keyboard1, 0xD1);
    port_write(keyboard0, 0x00);

    while (1) {}
}

int get_keyboard_input(char *out)
{
    uint8_t scancode = get_scancode();

    if (scancode == 0xE1) {
        get_scancode();
        get_scancode();
        *out = KEY_PAUSE;
        return 0;
    }

    if (scancode == 0xE0) {
        get_scancode();
        scancode = get_scancode();
    }

    update_global_keyboard_manager_state(scancode);

    if (scancode & 0x80) {
        return 1;
    }

    *out = convert_scancode_to_ascii(scancode);

    return 0;
}

bool is_capslock_on(void)
{
    return global_keyboard_manager_data.is_capslock_on;
}

bool is_numlock_on(void)
{
    return global_keyboard_manager_data.is_numlock_on;
}

bool is_scroll_lock_on(void)
{
    return global_keyboard_manager_data.is_scroll_lock_on;
}

bool is_shift_down(void)
{
    return global_keyboard_manager_data.is_shift_down;
}
