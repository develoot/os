#include "port.h"

uint8_t port_read(enum port port)
{
    uint8_t result;

    asm __volatile__(
        "mov  %1,   %%dx    \n\t"
        "in   %%dx, %0      \n\t"
        : "=r"(result)
        : "m"(port)
        : "dx"
    );

    return result;
}

void port_write(enum port port, uint8_t byte)
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
