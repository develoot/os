#include <stdarg.h>
#include "print.h"

static struct console_info global_console_info;

int init_print(struct graphic_frame_buffer_info frame_buffer_info, struct psf1_info psf1_info)
{
    global_console_info.frame_buffer_info = frame_buffer_info;
    global_console_info.psf1_info = psf1_info;

    global_console_info.cursor.x = 10;
    global_console_info.cursor.y = 10;

    global_console_info.pixel_block_size = 1;

    global_console_info.pixel_color.red = 0xff;
    global_console_info.pixel_color.green = 0xff;
    global_console_info.pixel_color.blue = 0xff;

    return 0;
}

void print_char(char character)
{
    static uint8_t row_masks[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

    struct graphic_frame_buffer_info *frame_buffer_info = &global_console_info.frame_buffer_info;
    struct psf1_info *psf1_info = &global_console_info.psf1_info;
    uint64_t block_size = global_console_info.pixel_block_size;
    uint8_t *glyph = &psf1_info->glyph_buffer[character * psf1_info->header.glyph_size];

    for (uint64_t y_offset = 0; y_offset < 16; ++y_offset) {
        for (uint64_t x_offset = 0; x_offset < 8; ++x_offset) {
            if (glyph[y_offset] & row_masks[x_offset]) { /* If nth bit of this row is 1. */
                draw_block(frame_buffer_info,
                        global_console_info.cursor.x + (x_offset * block_size),
                        global_console_info.cursor.y + (y_offset * block_size),
                        block_size, global_console_info.pixel_color);
            }
        }
    }

    global_console_info.cursor.x += block_size * 8;

    if (global_console_info.cursor.x >= frame_buffer_info->width) {
        global_console_info.cursor.x = 0;
        global_console_info.cursor.y += block_size * 16;
    }

    if (global_console_info.cursor.y >= frame_buffer_info->height) {
        print_clear();
        global_console_info.cursor.y = 0;
    }
}

void print_string(char *string)
{
    for (uint64_t i = 0; string[i] != '\0'; ++i) {
        print_char(string[i]);
    }
}

static char conversion_table[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

static void reverse_buffer(char *buffer, uint64_t size)
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

static void int_to_string(char *buffer, int value)
{
    uint64_t i = 0;

    do {
        buffer[i++] = conversion_table[value % 10];
        value /= 10;
    } while (value > 0);

    reverse_buffer(buffer, i);
    buffer[i] = '\0';
}

static void uint_to_string(char *buffer, unsigned int value)
{
    uint64_t i = 0;

    do {
        buffer[i++] = conversion_table[value % 10];
        value /= 10;
    } while (value > 0);

    reverse_buffer(buffer, i);
    buffer[i] = '\0';
}

static void long_to_string(char *buffer, long value)
{
    uint64_t i = 0;

    do {
        buffer[i++] = conversion_table[value % 10];
        value /= 10;
    } while (value > 0);

    reverse_buffer(buffer, i);
    buffer[i] = '\0';
}

static void ulong_to_string(char *buffer, unsigned long value)
{
    uint64_t i = 0;

    do {
        buffer[i++] = conversion_table[value % 10];
        value /= 10;
    } while (value > 0);

    reverse_buffer(buffer, i);
    buffer[i] = '\0';
}

#define PRINT_FORMAT_BUFFER_SIZE (1024)

void print_format(char *format, ...)
{
    /**
     * %c: character, 1 byte.
     * %s: pointer to string, 8 bytes.
     * %d: signed integer, 4 bytes.
     * %u: unsigned integer, 4 bytes.
     * %ld: signed long, 8 bytes.
     * %lu: unsigned long, 8 bytes.
     */

    static char buffer[PRINT_FORMAT_BUFFER_SIZE] = { 0 };

    va_list va;
    va_start(va, format);

    while (*format != '\0') {
        if (*format != '%') {
            print_char(*format);
            ++format;
            continue;
        }

        ++format;
        switch (*format) {
        case 'c':
            print_char(va_arg(va, int));
            ++format;
            break;
        case 's':
            print_string(va_arg(va, char *));
            ++format;
            break;
        case 'd':
            int_to_string(buffer, va_arg(va, int));
            print_string(buffer);
            ++format;
            break;
        case 'u':
            uint_to_string(buffer, va_arg(va, unsigned int));
            print_string(buffer);
            ++format;
            break;
        case 'l':
            ++format;
            switch (*format) {
            case 'd':
                long_to_string(buffer, va_arg(va, long));
                print_string(buffer);
                ++format;
                break;
            case 'u':
                ulong_to_string(buffer, va_arg(va, unsigned long));
                print_string(buffer);
                ++format;
                break;
            }
        }
    }

    va_end(va);
}

void print_clear(void)
{
    struct graphic_frame_buffer_info *frame_buffer_info = &global_console_info.frame_buffer_info;
    struct pixel_color black = { .blue = 0x00, .red = 0x00, .green = 0x00 };

    for (uint64_t i = 0; i < frame_buffer_info->height; ++i) {
        for (uint64_t j = 0; j < frame_buffer_info->width; ++j) {
            draw_block(frame_buffer_info, j, i, 1, black);
        }
    }
}
