#ifndef _KERNEL_CONSOLE_H
#define _KERNEL_CONSOLE_H

#include <stdint.h>
#include <drivers/graphic/screen.h>

#define PSF1_MAGIC0     (0x36)
#define PSF1_MAGIC1     (0x04)

#define PSF1_MODE256    (0x00) // 256 characters, no unicode.
#define PSF1_MODE512    (0x01) // 512 characters, no unicode.
#define PSF1_MODEHASTAB (0x02) // 256 characters, with unicode.
#define PSF1_MODEHASSEQ (0x03) // 512 characters, with unicode.

struct psf1_header {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t glyph_size;
};

struct psf1_data {
    struct psf1_header header;
    uint8_t *glyph_buffer;
};

struct console_cursor {
    uint64_t x;
    uint64_t y;
};

int console_initialize(struct graphic_frame_buffer_data frame_buffer_data,
        struct psf1_data psf1_data, struct pixel_color foreground_color,
        struct pixel_color background_color, uint64_t pixel_block_size);

void console_clear(void);

int console_print_char(char ch);

int console_print_string(const char *const string);

int console_print_format(const char *const format, ...);

void console_set_cursor(struct console_cursor cursor);

struct console_cursor console_get_cursor(void);

void console_set_foreground_color(struct pixel_color color);

void console_set_background_color(struct pixel_color color);

void console_set_pixel_block_size(uint64_t pixel_block_size);

uint64_t console_get_width(void);

uint64_t console_get_height(void);

#endif
