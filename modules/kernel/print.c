#include <stdarg.h>
#include <general/string.h>

#include "print.h"

#define PRINT_FORMAT_BUFFER_SIZE (1024)

static struct console_data global_console_data;

int print_initialize(struct graphic_frame_buffer_data frame_buffer_data, struct psf1_data psf1_data)
{
    global_console_data.frame_buffer_data = frame_buffer_data;
    global_console_data.psf1_data = psf1_data;

    global_console_data.cursor.x = 10;
    global_console_data.cursor.y = 10;

    global_console_data.pixel_block_size = 1;

    global_console_data.pixel_color.red = 0xff;
    global_console_data.pixel_color.green = 0xff;
    global_console_data.pixel_color.blue = 0xff;

    return 0;
}

void print_char(char character)
{
    static uint8_t row_masks[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

    struct graphic_frame_buffer_data *frame_buffer_data = &global_console_data.frame_buffer_data;
    struct psf1_data *psf1_data = &global_console_data.psf1_data;
    uint64_t block_size = global_console_data.pixel_block_size;
    uint8_t *glyph = &psf1_data->glyph_buffer[character * psf1_data->header.glyph_size];

    for (uint64_t y_offset = 0; y_offset < 16; ++y_offset) {
        for (uint64_t x_offset = 0; x_offset < 8; ++x_offset) {
            if (glyph[y_offset] & row_masks[x_offset]) { /* If nth bit of this row is 1. */
                screen_draw_block(frame_buffer_data,
                        global_console_data.cursor.x + (x_offset * block_size),
                        global_console_data.cursor.y + (y_offset * block_size),
                        block_size, global_console_data.pixel_color);
            }
        }
    }

    global_console_data.cursor.x += block_size * 8;

    if (global_console_data.cursor.x >= frame_buffer_data->width) {
        global_console_data.cursor.x = 0;
        global_console_data.cursor.y += block_size * 16;
    }

    if (global_console_data.cursor.y >= frame_buffer_data->height) {
        print_clear();
        global_console_data.cursor.y = 0;
    }
}

void print_string(const char *const string)
{
    for (uint64_t i = 0; string[i] != '\0'; ++i) {
        print_char(string[i]);
    }
}

int print_format(const char *const format, ...)
{
    char buffer[PRINT_FORMAT_BUFFER_SIZE];

    va_list ap;
    va_start(ap, format);
    int result = string_format_va(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    if (result != 0) {
        return -1;
    }

    print_string(buffer);

    return 0;
}

void print_clear(void)
{
    struct graphic_frame_buffer_data *frame_buffer_data = &global_console_data.frame_buffer_data;
    struct pixel_color black = { .blue = 0x00, .red = 0x00, .green = 0x00 };

    for (uint64_t i = 0; i < frame_buffer_data->height; ++i) {
        for (uint64_t j = 0; j < frame_buffer_data->width; ++j) {
            screen_draw_block(frame_buffer_data, j, i, 1, black);
        }
    }
}
