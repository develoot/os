#ifndef _INTERRUPTS_PROGRAMMABLE_INTERRUPT_CONTROLLER_H
#define _INTERRUPTS_PROGRAMMABLE_INTERRUPT_CONTROLLER_H

/** For the 8259A interrupt controller. */

/*
 * Initialize Command Word (ICW) definitions.
 *
 * Check the data sheet for detailed descriptions.
 */

#define ICW1       (0x11)

#define ICW2MASTER (0x20)
#define ICW2SLAVE  (0x28)

#define ICW3MASTER (0x04)
#define ICW3SLAVE  (0x02)

#define ICW4       (0x01)

#endif
