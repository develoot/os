#include "screen.h"

static inline uint32_t get_pixel(EFI_GRAPHICS_PIXEL_FORMAT pixel_format, struct pixel_color color)
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

static inline void draw_pixel(struct graphic_frame_buffer_data *buffer_data, uint64_t x, uint64_t y,
        struct pixel_color color)
{
    if (x > buffer_data->width || y > buffer_data->height) {
        return;
    }

    uint32_t *frame_buffer = (uint32_t *)buffer_data->address;

    frame_buffer[x + y * buffer_data->pixel_per_scanline]
        = get_pixel(buffer_data->pixel_format, color);
}

void screen_draw_block(struct graphic_frame_buffer_data *buffer_data, uint64_t x, uint64_t y,
        uint64_t block_size, struct pixel_color color)
{
    for (uint64_t i = 0; i < block_size; ++i) {
        for (uint64_t j = 0; j < block_size; ++j) {
            draw_pixel(buffer_data, x + j, y + i, color);
        }
    }
}
