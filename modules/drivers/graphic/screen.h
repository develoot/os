#ifndef _DRIVERS_GRAPHIC_SCREEN_H
#define _DRIVERS_GRAPHIC_SCREEN_H

#include <stdint.h>
#include <uefi/uefi.h>

struct pixel_color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct graphic_frame_buffer_data {
    uint64_t address;
    uint64_t size;
    uint64_t width;
    uint64_t height;
    uint64_t pixel_per_scanline;
    EFI_GRAPHICS_PIXEL_FORMAT pixel_format;
};

void screen_draw_block(struct graphic_frame_buffer_data *buffer_data, uint64_t x, uint64_t y,
        uint64_t block_size, struct pixel_color color);

#endif
