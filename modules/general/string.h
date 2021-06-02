#ifndef _GENERAL_STRING_H
#define _GENERAL_STRING_H

#include <stddef.h>
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
int string_format(char *buffer, uint64_t buffer_size, const char *format, ...);

size_t string_length(const char *const string);

#endif
