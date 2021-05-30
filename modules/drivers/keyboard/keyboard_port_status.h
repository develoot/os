#ifndef _DRIVERS_KEYBOARD_KEYBOARD_PORT_STATUS_H
#define _DRIVERS_KEYBOARD_KEYBOARD_PORT_STATUS_H

#define KEYBOARD_STATUS_OUTB (0x01) // Output buffer state. (0: empty, 1: full)
#define KEYBOARD_STATUS_INPB (0x02) // Input buffer state.
#define KEYBOARD_STATUS_SYSF (0x04) // System flag, (0: unavailage, 1: available)
#define KEYBOARD_STATUS_CD   (0x08) // Indicate type of the last byte sent. (0: command, 1: data)
#define KEYBOARD_STATUS_KEYL (0x10) // Keyboard lock status.
#define KEYBOARD_STATUS_AUXB (0x20) // Output has auxiliary device. (e.g, mouse)
#define KEYBOARD_STATUS_TIM  (0x40) // General time-out.
#define KEYBOARD_STATUS_PARE (0x80) // Parity error.

#endif
