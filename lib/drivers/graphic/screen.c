#include "screen.h"

static uint32_t get_pixel(EFI_GRAPHICS_PIXEL_FORMAT pixel_format, struct pixel_color color)
{
    switch (pixel_format) {
    case PixelRedGreenBlueReserved8BitPerColor:
        return (0x00 << 24) + (color.blue << 16) + (color.green << 8) + color.red;
    case PixelBlueGreenRedReserved8BitPerColor:
        return (0x00 << 24) + (color.red << 16) + (color.green << 8) + color.blue;
    default:
        return 0;
    }
}

static void draw_pixel(struct graphic_frame_buffer_info *buffer_info, uint64_t x, uint64_t y,
        struct pixel_color color)
{
    if (x > buffer_info->width || y > buffer_info->height) {
        return;
    }

    uint32_t *frame_buffer = (uint32_t *)buffer_info->address;

    frame_buffer[x + y * buffer_info->pixel_per_scanline]
        = get_pixel(buffer_info->pixel_format, color);
}

void draw_block(struct graphic_frame_buffer_info *buffer_info, uint64_t x, uint64_t y,
        uint64_t block_size, struct pixel_color color)
{
    for (uint64_t i = 0; i < block_size; ++i) {
        for (uint64_t j = 0; j < block_size; ++j) {
            draw_pixel(buffer_info, x + j, y + i, color);
        }
    }
}
