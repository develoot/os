#include "screen.h"

typedef uint32_t pixel_value;

static inline pixel_value get_pixel_value(EFI_GRAPHICS_PIXEL_FORMAT pixel_format, struct pixel_color pixel)
{
    switch (pixel_format) {
    case PixelRedGreenBlueReserved8BitPerColor:
        return (0x00 << 24) + (pixel.blue << 16) + (pixel.green << 8) + pixel.red;
    case PixelBlueGreenRedReserved8BitPerColor:
        return (0x00 << 24) + (pixel.red << 16) + (pixel.green << 8) + pixel.blue;
    default:
        return 0;
    }
}

static inline void draw_pixel(const struct graphic_frame_buffer_data *const buffer_data,
        uint64_t x, uint64_t y, struct pixel_color pixel)
{
    if (x >= buffer_data->width || y >= buffer_data->height) {
        return;
    }

    pixel_value *const frame_buffer = (pixel_value *)buffer_data->address;

    frame_buffer[x + (y * buffer_data->pixel_per_scanline)]
        = get_pixel_value(buffer_data->pixel_format, pixel);
}

void screen_draw_block(const struct graphic_frame_buffer_data *const buffer_data,
        uint64_t x, uint64_t y, const struct pixel_color color, const uint64_t block_size)
{
    for (uint64_t i = 0; i < block_size; ++i) {
        for (uint64_t j = 0; j < block_size; ++j) {
            draw_pixel(buffer_data, x + j, y + i, color);
        }
    }
}
