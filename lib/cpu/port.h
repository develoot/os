#ifndef _CPU_PORT_H
#define _CPU_PORT_H

#include <stdint.h>

enum port {
    keyboard_port0 = 0x60,
    keyboard_port1 = 0x64,

    // PIC stands for "Programmable Interrupt Controller".
    // For the 8259A interrupt controller.
    pic_master_port0 = 0x20,
    pic_master_port1 = 0x21,
    pic_slave_port0  = 0xA0,
    pic_slave_port1  = 0xA1
};

uint8_t read_port(enum port port);

void write_port(enum port port, uint8_t byte);

#endif
