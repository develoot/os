#include <cpu/port.h>

#include "programmable_interrupt_controller.h"

/** For the 8259A interrupt controller. */

/*
 * Command word definitions.
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

static inline void initialize_master_programmable_interrupt_controller(void)
{
    port_write(pic_master0, ICW1);
    port_wait();

    port_write(pic_master1, ICW2MASTER);
    port_wait();

    port_write(pic_master1, ICW3MASTER);
    port_wait();

    port_write(pic_master1, ICW4);
    port_wait();
}

static inline void initialize_slave_programmable_interrupt_controller(void)
{
    port_write(pic_slave0, ICW1);
    port_wait();

    port_write(pic_slave1, ICW2SLAVE);
    port_wait();

    port_write(pic_slave1, ICW3SLAVE);
    port_wait();

    port_write(pic_slave1, ICW4);
    port_wait();
}

void initialize_programmable_interrupt_controller(void)
{
    initialize_master_programmable_interrupt_controller();
    initialize_slave_programmable_interrupt_controller();
}

void set_interrupt_mask(uint16_t mask)
{
    // Check OCW1 command in the 8259A data sheet for detailed description.
    port_write(pic_master1, (uint8_t)mask);
    port_write(pic_slave1, (uint8_t)(mask >> 8));
}

void notify_end_of_interrupt(uint8_t interrupt_request_number)
{
    // Check OCW2 command in the 8259A data sheet for detailed description.
    port_write(pic_master1, OCW2);

    // You need to notify to the slave if the interrupt number is greater or equal to 8.
    // Consider the master-slave structure.
    if (interrupt_request_number >= 8) {
        port_write(pic_slave1, OCW2);
    }
}
