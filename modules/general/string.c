#include "string.h"

#define CONVERTED_STRING_BUFFER_SIZE (1024)

static const char conversion_table[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

static inline void reverse_buffer(char *const buffer, size_t size)
{
    char tmp;
    uint64_t i = 0;
    uint64_t j = size - 1;

    while (i < j) {
        tmp = buffer[i];
        buffer[i++] = buffer[j];
        buffer[j--] = tmp;
    }
}

static inline size_t convert_int_to_string(char *const buffer, int32_t value)
{
    uint64_t i = 0;

    do {
        buffer[i++] = conversion_table[value % 10];
        value /= 10;
    } while (value > 0);

    reverse_buffer(buffer, i);
    buffer[i] = '\0';

    return i;
}

static inline size_t convert_uint_to_string(char *const buffer, uint32_t value)
{
    uint64_t i = 0;

    do {
        buffer[i++] = conversion_table[value % 10];
        value /= 10;
    } while (value > 0);

    reverse_buffer(buffer, i);
    buffer[i] = '\0';

    return i;
}

static inline size_t convert_long_to_string(char *const buffer, int64_t value)
{
    uint64_t i = 0;

    do {
        buffer[i++] = conversion_table[value % 10];
        value /= 10;
    } while (value > 0);

    reverse_buffer(buffer, i);
    buffer[i] = '\0';

    return i;
}

static inline size_t convert_ulong_to_string(char *const buffer, uint64_t value)
{
    uint64_t i = 0;

    do {
        buffer[i++] = conversion_table[value % 10];
        value /= 10;
    } while (value > 0);

    reverse_buffer(buffer, i);
    buffer[i] = '\0';

    return i;
}

static inline size_t copy_string_to_buffer(char *const buffer, const char *const string)
{
    size_t i;
    for (i = 0; string[i] != '\0'; ++i) {
        buffer[i] = string[i];
    }
    return i;
}

size_t string_length(const char *const string)
{
    size_t i;
    for (i = 0; string[i] != '\0'; ++i);
    return i;
}

int string_format(char *output_buffer, size_t output_buffer_size, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = string_format_va(output_buffer, output_buffer_size, format, ap);
    va_end(ap);

    return result;
}

int string_format_va(char *output_buffer, uint64_t output_buffer_size, const char *format, va_list ap)
{
    char converted_string_buffer[CONVERTED_STRING_BUFFER_SIZE];
    const char *format_cursor = &format[0];
    uint64_t output_buffer_index = 0;

    while (*format_cursor != '\0') {
        if (*format_cursor != '%') {
            if (output_buffer_size <= output_buffer_index) {
                return -1;
            }
            output_buffer[output_buffer_index++] = *format_cursor;
            ++format_cursor;
            continue;
        }

        ++format_cursor;
        switch (*format_cursor) {
        case 'c': {
            if (output_buffer_size <= output_buffer_index) {
                return -1;
            }
            output_buffer[output_buffer_index++] = (char)va_arg(ap, int);
            ++format_cursor;
            break;
        }
        case 's': {
            const char *const string = va_arg(ap, char *);
            if (output_buffer_size < output_buffer_index + string_length(string)) {
                return -1;
            }
            output_buffer_index
                += copy_string_to_buffer(&output_buffer[output_buffer_index], string);
            ++format_cursor;
            break;
        }
        case 'd': {
            size_t string_size = convert_int_to_string(converted_string_buffer, va_arg(ap, int32_t));
            if (output_buffer_size < output_buffer_index + string_size) {
                return -1;
            }
            output_buffer_index +=
                copy_string_to_buffer(&output_buffer[output_buffer_index], converted_string_buffer);
            ++format_cursor;
            break;
        }
        case 'u': {
            size_t string_size =
                convert_uint_to_string(converted_string_buffer, va_arg(ap, uint32_t));
            if (output_buffer_size < output_buffer_index + string_size) {
                return -1;
            }
            output_buffer_index +=
                copy_string_to_buffer(&output_buffer[output_buffer_index], converted_string_buffer);
            ++format_cursor;
            break;
        }
        case 'l': {
            ++format_cursor;
            switch (*format_cursor) {
            case 'd': {
                size_t string_size =
                    convert_long_to_string(converted_string_buffer, va_arg(ap, int64_t));
                if (output_buffer_size < output_buffer_index + string_size) {
                    return -1;
                }
                output_buffer_index +=
                    copy_string_to_buffer(&output_buffer[output_buffer_index],
                            converted_string_buffer);
                ++format_cursor;
                break;
            }
            case 'u': {
                size_t string_size =
                    convert_ulong_to_string(converted_string_buffer, va_arg(ap, int64_t));
                if (output_buffer_size < output_buffer_index + string_size) {
                    return -1;
                }
                output_buffer_index +=
                    copy_string_to_buffer(&output_buffer[output_buffer_index],
                            converted_string_buffer);
                ++format_cursor;
                break;
            }
            default:
                break;
            }
            break;
        }
        default:
            break;
        }
    }

    if (output_buffer_size <= output_buffer_index) {
        return -1;
    }
    output_buffer[output_buffer_index] = '\0';

    return 0;
}
