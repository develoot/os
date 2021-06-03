#ifndef _GENERAL_STRING_H
#define _GENERAL_STRING_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Put formatted string into given `buffer`.
 *
 * %c:  Character, 1 byte.
 * %s:  Pointer to string, 8 bytes.
 * %d:  Signed integer, 4 bytes.
 * %u:  Unsigned integer, 4 bytes.
 * %ld: Signed long, 8 bytes.
 * %lu: Unsigned long, 8 bytes.
 */
int string_format(char *buffer, uint64_t buffer_size, const char *format, ...);

int string_format_va(char *const output_buffer, const uint64_t output_buffer_size,
        const char *const format, va_list ap);

size_t string_length(const char *const string);

#endif
