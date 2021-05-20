#include "scancode_to_ascii.h"
#include "keyboard_manager.h"

#define MAX_TRY (0xFFFFFFFF)

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

enum port {
    port0 = 0x60,
    port1 = 0x64
};

struct keyboard_manager_data global_keyboard_manager_data;

static uint8_t read_port(enum port port)
{
    uint8_t result;

    asm __volatile__(
            "mov  %1,   %%dx    \n\t"
            "in   %%dx, %0      \n\t"
            : "=r"(result)
            : "m"(port)
            : "dx"
    );

    return result;
}

static void write_port(enum port port, uint8_t byte)
{
    asm __volatile__(
            "mov  %0,   %%dx    \n\t"
            "mov  %1,   %%al    \n\t"
            "out  %%al, %%dx    \n\t"
            :
            : "m"(port), "m"(byte)
            : "dx", "al"
    );
}

static enum boolean is_output_buffer_full(void)
{
    return (read_port(port1) & KEYBOARD_STATUS_OUTB) > 0;
}

static enum boolean is_input_buffer_full(void)
{
    return (read_port(port1) & KEYBOARD_STATUS_INPB) > 0;
}

static uint8_t get_scancode(void)
{
    uint64_t try = 0;
    while (is_output_buffer_full() == false && try++ < MAX_TRY) {}

    return read_port(port0);
}

void init_keyboard_manager(void)
{
    global_keyboard_manager_data.is_capslock_on = false;
    global_keyboard_manager_data.is_numlock_on = false;
    global_keyboard_manager_data.is_scroll_lock_on = false;
    global_keyboard_manager_data.is_shift_down = false;
}

int change_keyboard_led(enum boolean is_capslock_on, enum boolean is_numlock_on,
        enum boolean is_scrolllock_on)
{
    uint64_t try;

    try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY) {}

    write_port(port0, KEYBOARD_CMD_CHANGE_LED);

    // Wait until keyboard processes CHANGE_LED command.
    try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY) {}

    for (uint64_t i = 0; i < 100; ++i) {
        try = 0;
        while (is_output_buffer_full() == false && try++ < MAX_TRY) {}

        if (read_port(port0) == KEYBOARD_CMD_ACK) {
            goto SUCCESS_FINDING_ACK;
        }
    }

    return 1;

SUCCESS_FINDING_ACK:

    write_port(port0, ((is_capslock_on << 2) | (is_numlock_on << 1) | (is_scrolllock_on)));

    try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY) {}

    for (uint64_t i = 0; i < 100; ++i) {
        try = 0;
        while (is_output_buffer_full() == false && try++ < MAX_TRY) {}

        if (read_port(port0) == KEYBOARD_CMD_ACK) {
            return 0;
        }
    }

    return 1;
}

void update_global_keyboard_manager_state(uint8_t scancode)
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

int activate_keyboard(void)
{
    uint64_t try;

    write_port(port1, KEYBOARD_CMD_ACTIVATE_CONTROLLER);

    // Wait until keyboard processes input so that input buffer is empty.
    // TODO: This implementation is dumb.
    try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY) {}

    write_port(port0, KEYBOARD_CMD_ACTIVATE_KEYBOARD);

    // Read 100 times from output buffer to find ACK message.
    for (uint64_t i = 0; i < 100; ++i) {
        try = 0;
        while (is_output_buffer_full() == false && try++ < MAX_TRY) {}

        if (read_port(port0) == KEYBOARD_CMD_ACK) {
            return 0;
        }
    }

    return 1;
}

void enable_a20_gate(void)
{
    uint64_t try;

    write_port(port1, KEYBOARD_CMD_READ_CONTROLLER_OUT);

    try = 0;
    while (is_output_buffer_full() == false && try++ < MAX_TRY) {}

    uint8_t data = read_port(port0);
    data |= 0x01; // Set "Enable A20" bit.

    try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY) {}

    write_port(port1, KEYBOARD_CMD_SET_OUTPUT_PORT);
    write_port(port0, data);
}

void reset_processor(void)
{
    uint64_t try = 0;
    while (is_input_buffer_full() == true && try++ < MAX_TRY);

    write_port(port1, 0xD1);
    write_port(port0, 0x00);

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
