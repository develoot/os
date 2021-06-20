#ifndef _KERNEL_SHELL_H
#define _KERNEL_SHELL_H

#include <stddef.h>
#include <general/byte.h>

int shell_insert(const byte_t *const data, size_t size);

int shell_start(void);

#endif
