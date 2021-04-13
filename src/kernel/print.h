#ifndef _KERNEL_PRINT_H
#define _KERNEL_PRINT_H

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

struct psf1_info {
    struct psf1_header header;
    uint8_t *glyph_buffer;
};

struct console_cursor {
    uint64_t x;
    uint64_t y;
};

struct console_info {
    struct psf1_info psf1_info;
    struct graphic_frame_buffer_info frame_buffer_info;
    struct pixel_color pixel_color;
    struct console_cursor cursor;
    uint64_t pixel_block_size;
};

/**
 * Initialize global variable `global_console_info`.
 *
 * Our loader is so dumb that it can't initialize global variables.
 * And gcc use `memset()` to initialize struct variable of which symbol can not be resolved.
 * So we have to initialize the variable manually here.
 */
int init_print(struct graphic_frame_buffer_info frame_buffer_info, struct psf1_info psf1_info);

void print_char(char character);
void print_string(char *string);
void print_format(char *format, ...);
void print_clear(void);

#endif
