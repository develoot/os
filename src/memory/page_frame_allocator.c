/** TODO: Zoned page frame allocator? */

#include <debug/assert.h>
#include <general/boolean.h>

#include "page_frame_allocator.h"

struct page_frame_allocator_info global_page_frame_allocator_info;

static inline uint64_t get_page_frame_index(page_frame_t page_frame)
{
    uint64_t page_frame_address = (uint64_t)page_frame;
    assert(page_frame_address % PAGE_FRAME_SIZE == 0, "Not aligned page frame");

    return page_frame_address / PAGE_FRAME_SIZE;
}

static inline uint64_t get_page_frame_address(uint64_t page_frame_index)
{
    assert(page_frame_index < PAGE_FRAME_BITMAP_MAX_PAGE_NUMBER,
            "Page frame index out of boundary");

    return page_frame_index * PAGE_FRAME_SIZE;
}

static uint8_t bit_masks[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

static inline void set_bit(uint8_t *bitmap, uint64_t bit_index, enum boolean value)
{
    uint64_t bitmap_index = bit_index / 8;
    bitmap[bitmap_index] &= ~bit_masks[bit_index % 8];
    bitmap[bitmap_index] |= (value << (bit_index % 8));
}

static inline void set_bits(uint8_t *bitmap, uint64_t bit_index, uint64_t size, enum boolean value)
{
    for (uint64_t i = 0; i < size; ++i) {
        set_bit(bitmap, bit_index + i, value);
    }
}

static inline enum boolean get_bit(uint8_t *bitmap, uint64_t bit_index)
{
    uint64_t bitmap_index = bit_index / 8;
    return (bitmap[bitmap_index] & bit_masks[bit_index % 8]) > 0;
}

static inline uint64_t get_total_page_frame_number(struct uefi_memory_map_info memory_map_info)
{
    uint64_t total_page_frame_number = 0;

    FOR_EACH_DESCRIPTOR(
            d,
            memory_map_info.memory_descriptor_buffer,
            memory_map_info.memory_descriptor_buffer_size,
            memory_map_info.memory_descriptor_size)
    {
        total_page_frame_number += d->NumberOfPages;
    }

    return total_page_frame_number;
}

static inline int init_page_frame_bitmap(uint8_t *bitmap,
        struct uefi_memory_map_info memory_map_info)
{
    uint64_t bit_index = 0;

    FOR_EACH_DESCRIPTOR(
            d,
            memory_map_info.memory_descriptor_buffer,
            memory_map_info.memory_descriptor_buffer_size,
            memory_map_info.memory_descriptor_size)
    {
        switch (d->Type) {
        case EfiReservedMemoryType:
        case EfiLoaderCode:
        case EfiLoaderData:
        case EfiRuntimeServicesCode:
        case EfiRuntimeServicesData:
        case EfiUnusableMemory:
        case EfiACPIReclaimMemory:
        case EfiACPIMemoryNVS:
        case EfiMemoryMappedIO:
        case EfiMemoryMappedIOPortSpace:
        case EfiPalCode:
            break;

        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiConventionalMemory:
            bit_index = get_page_frame_index((page_frame_t)d->PhysicalStart);
            set_bits(bitmap, bit_index, d->NumberOfPages, false);
            break;

        default:
            return 1;
        }
    }

    return 0;
}

int init_page_frame_allocator(struct uefi_memory_map_info memory_map_info)
{
    uint64_t total_page_frame_number = get_total_page_frame_number(memory_map_info);
    if (total_page_frame_number > PAGE_FRAME_BITMAP_MAX_PAGE_NUMBER) {
        total_page_frame_number = PAGE_FRAME_BITMAP_MAX_PAGE_NUMBER;
    }

    global_page_frame_allocator_info.bitmap_size = (total_page_frame_number % 8) > 0
            ? (total_page_frame_number / 8) + 1
            : (total_page_frame_number / 8);

    for (uint64_t i = 0; i < global_page_frame_allocator_info.bitmap_size; ++i) {
        global_page_frame_allocator_info.bitmap[i] = 0xFF;
    }

    global_page_frame_allocator_info.total_page_frame_number = total_page_frame_number;
    global_page_frame_allocator_info.free_page_frame_number = total_page_frame_number;

    int result = init_page_frame_bitmap(global_page_frame_allocator_info.bitmap, memory_map_info);
    if (result != 0) {
        return 1;
    }

    return 0;
}

page_frame_t request_page_frame(void)
{
    page_frame_t new_page_frame = PAGE_FRAME_NULL;
    uint8_t *bitmap = global_page_frame_allocator_info.bitmap;

    if (global_page_frame_allocator_info.free_page_frame_number == 0) {
        return PAGE_FRAME_NULL;
    }

    for (uint64_t bit_index = 0;
            bit_index < global_page_frame_allocator_info.total_page_frame_number;
            ++bit_index)
    {
        if (get_bit(bitmap, bit_index) == true) {
            continue;
        }

        set_bit(bitmap, bit_index, true);
        --global_page_frame_allocator_info.free_page_frame_number;
        new_page_frame = (page_frame_t)get_page_frame_address(bit_index);
        break;
    }

    return new_page_frame;
}

void free_page_frame(page_frame_t page_frame)
{
    uint64_t page_frame_address = (uint64_t)page_frame;
    assert(page_frame_address % PAGE_FRAME_SIZE == 0, "Not aligned page frame");

    uint8_t *bitmap = global_page_frame_allocator_info.bitmap;
    uint64_t page_frame_index = get_page_frame_index(page_frame);
    set_bit(bitmap, page_frame_index, false);
    ++global_page_frame_allocator_info.free_page_frame_number;
}
