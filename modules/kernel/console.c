#include <stdarg.h>
#include <general/string.h>

#include "console.h"

#define PRINT_FORMAT_BUFFER_SIZE (1024)
#define PRINT_TAB_SIZE    (4)
#define PSF1_GLYPH_HEIGHT (16)
#define PSF1_GLYPH_WIDTH  (8)

struct cursor {
    uint64_t x;
    uint64_t y;
};

struct console_data {
    struct cursor cursor;
    struct graphic_frame_buffer_data frame_buffer_data;
    struct pixel_color foreground_color;
    struct pixel_color background_color;
    struct psf1_data psf1_data;
    uint64_t pixel_block_size;
};

static struct console_data global_console_data;

static void console_clear(void)
{
    struct graphic_frame_buffer_data *const frame_buffer_data = &global_console_data.frame_buffer_data;

    for (uint64_t i = 0; i < frame_buffer_data->height; ++i) {
        for (uint64_t j = 0; j < frame_buffer_data->width; ++j) {
            screen_draw_block(frame_buffer_data, j, i, global_console_data.background_color, 1);
        }
    }
}

int console_initialize(struct graphic_frame_buffer_data frame_buffer_data,
        struct psf1_data psf1_data, struct pixel_color foreground_color,
        struct pixel_color background_color, uint64_t pixel_block_size)
{
    global_console_data.cursor.x = 0;
    global_console_data.cursor.y = 0;

    global_console_data.frame_buffer_data = frame_buffer_data;

    global_console_data.foreground_color = foreground_color;
    global_console_data.background_color = background_color;

    global_console_data.psf1_data = psf1_data;

    global_console_data.pixel_block_size = pixel_block_size;

    return 0;
}

int console_print_char(char ch)
{
    const uint64_t pixel_block_size = global_console_data.pixel_block_size;
    struct cursor *const cursor = &global_console_data.cursor;
    struct graphic_frame_buffer_data *const frame_buffer_data = &global_console_data.frame_buffer_data;
    struct psf1_data *const psf1_data = &global_console_data.psf1_data;

    if (ch == '\n') {
        cursor->x = 0;
        cursor->y += pixel_block_size * PSF1_GLYPH_HEIGHT;
        return 0;
    }

    if (ch == '\t') {
        cursor->x += pixel_block_size * PSF1_GLYPH_WIDTH * PRINT_TAB_SIZE;
        return 0;
    }

    if (cursor->x >= frame_buffer_data->width) {
        cursor->x = 0;
        cursor->y += pixel_block_size * PSF1_GLYPH_HEIGHT;
    }

    if (cursor->y >= frame_buffer_data->height) {
        // TODO:
        console_clear();
        cursor->x = 0;
        cursor->y = 0;
    }

    const uint8_t *const glyph = &psf1_data->glyph_buffer[ch * psf1_data->header.glyph_size];

    for (uint64_t y = 0; y < PSF1_GLYPH_HEIGHT; ++y) {
        for (uint64_t x = 0; x < PSF1_GLYPH_WIDTH; ++x) {
            if ((glyph[y] << x) & 0x80) {
                screen_draw_block(frame_buffer_data,
                        cursor->x + (x * pixel_block_size), cursor->y + (y * pixel_block_size),
                        global_console_data.foreground_color, pixel_block_size); }
        }
    }

    cursor->x += pixel_block_size * PSF1_GLYPH_WIDTH;

    return 0;
}

int console_print_string(const char *const string)
{
    for (uint64_t i = 0; string[i] != '\0'; ++i) {
        console_print_char(string[i]);
    }

    return 0;
}

int console_print_format(const char *const format, ...)
{
    char buffer[PRINT_FORMAT_BUFFER_SIZE];

    va_list ap;
    va_start(ap, format);
    int result = string_format_va(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    if (result != 0) {
        return -1;
    }

    console_print_string(buffer);

    return 0;
}
