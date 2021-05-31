#ifndef _CPU_PORT_H
#define _CPU_PORT_H

#include <stdint.h>

#include <general/inline.h>

enum port {
    keyboard0 = 0x60,
    keyboard1 = 0x64,

    // PIC stands for "Programmable Interrupt Controller".
    // For the 8259A interrupt controller.
    pic_master0 = 0x20,
    pic_master1 = 0x21,
    pic_slave0  = 0xA0,
    pic_slave1  = 0xA1
};

uint8_t read_port(enum port port);

void write_port(enum port port, uint8_t byte);

always_inline void port_wait(void)
{
    // Waste one port I/O cycle by writing to unused port.
    asm __volatile__("outb %%al, $0x80" : : : "al");
}

#endif
