#ifndef _CPU_PORT_H
#define _CPU_PORT_H

#include <stdint.h>

enum port {
    keyboard_port0 = 0x60,
    keyboard_port1 = 0x64
};

uint8_t read_port(enum port port);

void write_port(enum port port, uint8_t byte);

#endif
