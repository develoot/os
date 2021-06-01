#ifndef _GENERAL_STRING_H
#define _GENERAL_STRING_H

#include <stdint.h>

/**
 * Put formateed string into the `buffer`.
 *
 * %c: character, 1 byte.
 * %s: pointer to string, 8 bytes.
 * %d: signed integer, 4 bytes.
 * %u: unsigned integer, 4 bytes.
 * %ld: signed long, 8 bytes.
 * %lu: unsigned long, 8 bytes.
 */
int format_string(char *buffer, uint64_t buffer_size, const char *format, ...);

#endif
