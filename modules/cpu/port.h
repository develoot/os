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

always_inline void port_wait(void)
{
    // Waste one port I/O cycle by writing to unused port.
    asm __volatile__("outb %%al, $0x80" : : : "al");
}

always_inline uint8_t port_read(enum port port)
{
    uint8_t result;

    asm __volatile__(
        "mov %1, %%dx \n\t"
        "in  %%dx, %0 \n\t"
        : "=r"(result)
        : "m"(port)
        : "dx"
    );

    return result;
}

always_inline void port_write(enum port port, uint8_t byte)
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

#endif
