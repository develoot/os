#include <cpu/port.h>

#include "programmable_interrupt_controller.h"

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

#define OCW2       (0x20)

void initialize_programmable_interrupt_controller(void)
{
    write_port(pic_master0, ICW1);
    write_port(pic_master1, ICW2MASTER);
    write_port(pic_master1, ICW3MASTER);
    write_port(pic_master1, ICW4);

    write_port(pic_slave0, ICW1);
    write_port(pic_slave1, ICW2SLAVE);
    write_port(pic_slave1, ICW3SLAVE);
    write_port(pic_slave1, ICW4);
}

void set_interrupt_mask(uint16_t mask)
{
    // Check OCW1 command in the 8259A data sheet for detailed description.
    write_port(pic_master1, (uint8_t)mask);
    write_port(pic_slave1, (uint8_t)(mask >> 8));
}

void notify_end_of_interrupt(uint8_t interrupt_request_number)
{
    // Check OCW2 command in the 8259A data sheet for detailed description.
    write_port(pic_master1, OCW2);

    // You need to notify to the slave if the interrupt number is greater or equal to 8.
    // Consider the master-slave structure.
    if (interrupt_request_number >= 8) {
        write_port(pic_slave1, OCW2);
    }
}
