#include <cpu/port.h>

#include "port.h"
#include "interrupt_handler.h"
#include "scancode_to_ascii.h"
#include "manager.h"

#define KEYBOARD_COMMAND_ACTIVATE_CONTROLLER  (0xAE)
#define KEYBOARD_COMMAND_ACTIVATE_KEYBOARD    (0xF4)
#define KEYBOARD_COMMAND_ACK                  (0xFA)
#define KEYBOARD_COMMAND_CHANGE_LED           (0xED)
#define KEYBOARD_COMMAND_READ_CONTROLLER_OUT  (0xD0)
#define KEYBOARD_COMMAND_SET_OUTPUT_PORT      (0xD1)

#define KEYBOARD_SCANCODE_LSHIFT_DOWN         (0x2A)
#define KEYBOARD_SCANCODE_LSHIFT_UP           (0xAA)
#define KEYBOARD_SCANCODE_RSHIFT_DOWN         (0x36)
#define KEYBOARD_SCANCODE_RSHIFT_UP           (0xB6)
#define KEYBOARD_SCANCODE_CAPSLOCK_DOWN       (0x3A)
#define KEYBOARD_SCANCODE_CAPSLOCK_UP         (0xBA)
#define KEYBOARD_SCANCODE_NUMLOCK_DOWN        (0x45)
#define KEYBOARD_SCANCODE_NUMLOCK_UP          (0xC5)
#define KEYBOARD_SCANCODE_SCROLLLOCK_DOWN     (0x46)
#define KEYBOARD_SCANCODE_SCROLLLOCK_UP       (0xC6)

static inline void wait_while_input_buffer_is_full(void)
{
    while (is_input_buffer_full() == true);
}

static inline void wait_while_keyboard_interrupt_handler_queue_is_empty(void)
{
    while (keyboard_interrupt_handler_queue_is_empty() == true);
}

static inline void write_command_on_port0(uint8_t command)
{
    wait_while_input_buffer_is_full();
    port_write(keyboard0, command);
}

static inline scancode_t get_scancode(void)
{
    wait_while_keyboard_interrupt_handler_queue_is_empty();
    return keyboard_interrupt_handler_get_scancode();
}

static struct keyboard_manager_data global_keyboard_manager_data;

static int change_keyboard_led(bool is_capslock_on, bool is_numlock_on, bool is_scroll_lock_on)
{
    write_command_on_port0(KEYBOARD_COMMAND_CHANGE_LED);

    for (uint64_t i = 0; i < 100; ++i) {
        if (get_scancode() == KEYBOARD_COMMAND_ACK) {
            goto SUCCESS_FINDING_ACK;
        }
    }

    return -1;

SUCCESS_FINDING_ACK:
    write_command_on_port0((is_capslock_on << 2) | (is_numlock_on << 1) | (is_scroll_lock_on));

    for (uint64_t i = 0; i < 100; ++i) {
        if (get_scancode() == KEYBOARD_COMMAND_ACK) {
            return 0;
        }
    }

    return -1;
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
    } else if (scancode == KEYBOARD_SCANCODE_NUMLOCK_DOWN) {
        global_keyboard_manager_data.is_numlock_on = !global_keyboard_manager_data.is_numlock_on;
    } else if (scancode == KEYBOARD_SCANCODE_NUMLOCK_DOWN) {
        global_keyboard_manager_data.is_scroll_lock_on =
            !global_keyboard_manager_data.is_scroll_lock_on;
    } else {
        return;
    }

    change_keyboard_led(global_keyboard_manager_data.is_capslock_on,
            global_keyboard_manager_data.is_numlock_on,
            global_keyboard_manager_data.is_scroll_lock_on);
}

void keyboard_manager_initialize(void)
{
    global_keyboard_manager_data.is_capslock_on    = false;
    global_keyboard_manager_data.is_numlock_on     = false;
    global_keyboard_manager_data.is_scroll_lock_on = false;
    global_keyboard_manager_data.is_shift_down     = false;
}

int keyboard_manager_activate_keyboard(void)
{
    keyboard_interrupt_handler_initialize();

    port_write(keyboard1, KEYBOARD_COMMAND_ACTIVATE_CONTROLLER);
    write_command_on_port0(KEYBOARD_COMMAND_ACTIVATE_KEYBOARD);

    // Read 100 times from output buffer to find ACK message.
    for (uint64_t i = 0; i < 100; ++i) {
        if (get_scancode() == KEYBOARD_COMMAND_ACK) {
            return 0;
        }
    }

    return -1;
}

void keyboard_manager_enable_a20(void)
{
    port_write(keyboard1, KEYBOARD_COMMAND_READ_CONTROLLER_OUT);
    uint8_t data = get_scancode();
    data |= 0x01; // Set "Enable A20" bit.

    port_write(keyboard1, KEYBOARD_COMMAND_SET_OUTPUT_PORT);
    write_command_on_port0(data);
}

void keyboard_manager_reset_processor(void)
{
    port_write(keyboard1, 0xD1);
    write_command_on_port0(0x00);
}

int keyboard_manager_get_input(char *out)
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
        return -1;
    }

    *out = scancode_to_ascii_convert(scancode);

    return 0;
}

bool keyboard_manager_is_capslock_on(void)
{
    return global_keyboard_manager_data.is_capslock_on;
}

bool keyboard_manager_is_numlock_on(void)
{
    return global_keyboard_manager_data.is_numlock_on;
}

bool keyboard_manager_is_scroll_lock_on(void)
{
    return global_keyboard_manager_data.is_scroll_lock_on;
}

bool keyboard_manager_is_shift_down(void)
{
    return global_keyboard_manager_data.is_shift_down;
}
