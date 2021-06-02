#ifndef _GENERAL_STRING_H
#define _GENERAL_STRING_H

#include <stdarg.h>
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
int string_format(char *output_buffer, uint64_t output_buffer_size, const char *format, ...);

int string_format_va(char *output_buffer, uint64_t output_buffer_size, const char *format, va_list ap);

size_t string_length(const char *const string);

#endif
