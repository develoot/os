#include "string.h"

#define NUMBER_STRING_BUFFER_SIZE (1024)

static const char digit_to_char_table[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

static inline void swap(char *const a, char *const b)
{
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

static inline void reverse_buffer(char *const buffer, size_t size)
{
    uint64_t i = 0;
    uint64_t j = size - 1;

    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
}

static inline size_t int32_to_string(char *const buffer, const int32_t value)
{
    uint64_t size = 0;
    int32_t absolute_value = value < 0 ? value * -1 : value;

    do {
        buffer[size++] = digit_to_char_table[absolute_value % 10];
        absolute_value /= 10;
    } while (absolute_value > 0);

    if (value < 0) {
        buffer[size++] = '-';
    }

    reverse_buffer(buffer, size);
    buffer[size] = '\0';

    return size;
}

static inline size_t uint32_to_string(char *const buffer, uint32_t value)
{
    uint64_t i = 0;

    do {
        buffer[i++] = digit_to_char_table[value % 10];
        value /= 10;
    } while (value > 0);

    reverse_buffer(buffer, i);
    buffer[i] = '\0';

    return i;
}

static inline size_t int64_to_string(char *const buffer, int64_t value)
{
    uint64_t size = 0;
    int64_t absolute_value = value < 0 ? value * -1 : value;

    do {
        buffer[size++] = digit_to_char_table[absolute_value % 10];
        absolute_value /= 10;
    } while (absolute_value > 0);

    if (value < 0) {
        buffer[size++] = '-';
    }

    reverse_buffer(buffer, size);
    buffer[size] = '\0';

    return size;
}

static inline size_t uint64_to_string(char *const buffer, uint64_t value)
{
    uint64_t i = 0;

    do {
        buffer[i++] = digit_to_char_table[value % 10];
        value /= 10;
    } while (value > 0);

    reverse_buffer(buffer, i);
    buffer[i] = '\0';

    return i;
}

static inline size_t copy_string(char *const buffer, const char *const string)
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

int string_format(char *buffer, size_t buffer_size, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = string_format_va(buffer, buffer_size, format, ap);
    va_end(ap);

    return result;
}

int string_format_va(char *const output_buffer, const uint64_t output_buffer_size,
        const char *const format, va_list ap)
{
    char number_string_buffer[NUMBER_STRING_BUFFER_SIZE];
    const char *cursor = &format[0];
    uint64_t output_buffer_index = 0;

    while (*cursor != '\0') {
        if (output_buffer_size <= output_buffer_index) {
            return -1;
        }

        if (*cursor != '%') {
            output_buffer[output_buffer_index++] = *cursor;
            ++cursor;
            continue;
        }

        ++cursor;

        switch (*cursor) {
        case 'c':
            output_buffer[output_buffer_index++] = (char)va_arg(ap, int);
            ++cursor;
            break;
        case 's': {
            const char *const string = va_arg(ap, char *);
            if (output_buffer_size < output_buffer_index + string_length(va_arg(ap, char *))) {
                return -1;
            }
            output_buffer_index += copy_string(&output_buffer[output_buffer_index], string);
            ++cursor;
            break;
        }
        case 'd': {
            size_t string_size = int32_to_string(number_string_buffer, va_arg(ap, int32_t));
            if (output_buffer_size < output_buffer_index + string_size) {
                return -1;
            }
            output_buffer_index += copy_string(&output_buffer[output_buffer_index],
                    number_string_buffer);
            ++cursor;
            break;
        }
        case 'u': {
            size_t string_size = uint32_to_string(number_string_buffer, va_arg(ap, uint32_t));
            if (output_buffer_size < output_buffer_index + string_size) {
                return -1;
            }
            output_buffer_index += copy_string(&output_buffer[output_buffer_index],
                    number_string_buffer);
            ++cursor;
            break;
        }
        case 'l': {
            ++cursor;

            switch (*cursor) {
            case 'd': {
                size_t string_size = int64_to_string(number_string_buffer, va_arg(ap, int64_t));
                if (output_buffer_size < output_buffer_index + string_size) {
                    return -1;
                }
                output_buffer_index += copy_string(&output_buffer[output_buffer_index],
                        number_string_buffer);
                ++cursor;
                break;
            }
            case 'u': {
                size_t string_size = uint64_to_string(number_string_buffer, va_arg(ap, uint64_t));
                if (output_buffer_size < output_buffer_index + string_size) {
                    return -1;
                }
                output_buffer_index += copy_string(&output_buffer[output_buffer_index],
                        number_string_buffer);
                ++cursor;
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

int string_compare(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        if (*a != *b) {
            return -1;
        }

        ++a;
        ++b;
    }

    return 0;
}
