#ifndef _DRIVERS_KEYBOARD_PORT_H
#define _DRIVERS_KEYBOARD_PORT_H

#include <stdbool.h>
#include <cpu/port.h>

#define KEYBOARD_STATUS_OUTB (0x01) // Output buffer state. (0: empty, 1: full)
#define KEYBOARD_STATUS_INPB (0x02) // Input buffer state.
#define KEYBOARD_STATUS_SYSF (0x04) // System flag, (0: unavailage, 1: available)
#define KEYBOARD_STATUS_CD   (0x08) // Indicate type of the last byte sent. (0: command, 1: data)
#define KEYBOARD_STATUS_KEYL (0x10) // Keyboard lock status.
#define KEYBOARD_STATUS_AUXB (0x20) // Output has auxiliary device. (e.g, mouse)
#define KEYBOARD_STATUS_TIM  (0x40) // General time-out.
#define KEYBOARD_STATUS_PARE (0x80) // Parity error.

static inline bool is_output_buffer_full(void)
{
    return (port_read(keyboard1) & KEYBOARD_STATUS_OUTB) > 0;
}

static inline bool is_input_buffer_full(void)
{
    return (port_read(keyboard1) & KEYBOARD_STATUS_INPB) > 0;
}

#endif
