/** TODO: Implement zoned page frame allocator. */

#include <debug/assert.h>
#include <general/address.h>
#include <general/boolean.h>

#include "page_frame_allocator.h"

#define PAGE_FRAME_BITMAP_MAX_PAGE_NUMBER (PAGE_FRAME_BITMAP_MAX_SIZE * 8)
#define PAGE_FRAME_BITMAP_MAX_SIZE (32 * 1024 * 512) // 512 GB address space.

struct page_frame_allocator_data {
    /**
     * A bitmap that represents status of page frames.
     *
     * Each bits in this array represents status of the page frame.
     *
     * A page frame is in use if the bit is set. It's available otherwise.
     */
    uint8_t  bitmap[PAGE_FRAME_BITMAP_MAX_SIZE];
    uint64_t bitmap_size;
    uint64_t total_page_frame_number;
    uint64_t free_page_frame_number;
};

static struct page_frame_allocator_data global_page_frame_allocator_data;

static inline uint64_t convert_address_to_index(address_t page_frame_address)
{
    assert(page_frame_address % PAGE_FRAME_SIZE == 0, "Not aligned page frame");
    return page_frame_address / PAGE_FRAME_SIZE;
}

static inline address_t convert_index_to_address(uint64_t page_frame_index)
{
    assert(page_frame_index < PAGE_FRAME_BITMAP_MAX_PAGE_NUMBER, "Page frame index too large");
    return page_frame_index * PAGE_FRAME_SIZE;
}

static inline void set_bit(uint8_t *const bitmap, uint64_t bit_index, enum boolean value)
{
    bitmap[bit_index / 8] &= ~(1 << (bit_index % 8));
    bitmap[bit_index / 8] |= (value << (bit_index % 8));
}

static inline void set_bits(uint8_t *const bitmap, uint64_t bit_index, uint64_t size,
        enum boolean value)
{
    for (uint64_t i = 0; i < size; ++i) {
        set_bit(bitmap, bit_index + i, value);
    }
}

static inline enum boolean get_bit(uint8_t *const bitmap, uint64_t bit_index)
{
    return (bitmap[bit_index / 8] & (1 << (bit_index % 8))) > 0;
}

static uint64_t get_total_uefi_frame_number(struct uefi_memory_map_data memory_map_data)
{
    uint64_t total_uefi_frame_number = 0;

    FOR_EACH_DESCRIPTOR(
            descriptor,
            memory_map_data.memory_descriptor_buffer,
            memory_map_data.memory_descriptor_buffer_size,
            memory_map_data.memory_descriptor_size) {
        total_uefi_frame_number += descriptor->NumberOfPages;
    }

    return total_uefi_frame_number;
}

static int initialize_page_frame_bitmap(uint8_t *bitmap,
        struct uefi_memory_map_data memory_map_data)
{
    FOR_EACH_DESCRIPTOR(
            descriptor,
            memory_map_data.memory_descriptor_buffer,
            memory_map_data.memory_descriptor_buffer_size,
            memory_map_data.memory_descriptor_size) {
        switch (descriptor->Type) {
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
        case EfiPalCode: {
            break;
        }
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiConventionalMemory: {
            uint64_t bit_index = convert_address_to_index(descriptor->PhysicalStart);
            set_bits(bitmap, bit_index, descriptor->NumberOfPages, false);
            break;
        }
        default:
            return 1;
        }
    }

    return 0;
}

int initialize_page_frame_allocator(struct uefi_memory_map_data memory_map_data)
{
    uint64_t total_uefi_frame_number = get_total_uefi_frame_number(memory_map_data);
    if (total_uefi_frame_number <= 0) {
        return 1;
    }
    if (total_uefi_frame_number > PAGE_FRAME_BITMAP_MAX_PAGE_NUMBER) {
        total_uefi_frame_number = PAGE_FRAME_BITMAP_MAX_PAGE_NUMBER;
    }

    global_page_frame_allocator_data.bitmap_size = ((total_uefi_frame_number - 1) / 8) + 1;

    for (uint64_t i = 0; i < global_page_frame_allocator_data.bitmap_size; ++i) {
        global_page_frame_allocator_data.bitmap[i] = 0xFF;
    }

    global_page_frame_allocator_data.total_page_frame_number = total_uefi_frame_number;
    global_page_frame_allocator_data.free_page_frame_number  = total_uefi_frame_number;

    int result = initialize_page_frame_bitmap(global_page_frame_allocator_data.bitmap,
            memory_map_data);
    if (result != 0) {
        return 1;
    }

    return 0;
}

uint64_t get_total_page_frame_number(void)
{
    return global_page_frame_allocator_data.total_page_frame_number;
}

page_frame_t request_page_frames(uint64_t size)
{
    if (global_page_frame_allocator_data.free_page_frame_number < size) {
        return PAGE_FRAME_NULL;
    }

    page_frame_t new_page_frame = PAGE_FRAME_NULL;
    uint8_t *const bitmap = global_page_frame_allocator_data.bitmap;

    uint64_t current_page_frame_size = 0;
    for (uint64_t bit_index = 0;
            bit_index < global_page_frame_allocator_data.total_page_frame_number;
            bit_index++) {
        if (get_bit(bitmap, bit_index) == true) {
            current_page_frame_size = 0;
            continue;
        }
        current_page_frame_size++;
        if (current_page_frame_size == size) {
            set_bits(bitmap, bit_index - size + 1, size, true);
            global_page_frame_allocator_data.free_page_frame_number -= size;
            new_page_frame = (page_frame_t)convert_index_to_address(bit_index - size + 1);
            break;
        }
    }

    return new_page_frame;
}

void free_page_frames(page_frame_t page_frame, uint64_t size)
{
    address_t page_frame_address = (address_t)page_frame;
    assert(page_frame_address % PAGE_FRAME_SIZE == 0, "Not aligned page frame");

    uint8_t *const bitmap = global_page_frame_allocator_data.bitmap;
    uint64_t page_frame_index = convert_address_to_index(page_frame_address);
    for (uint64_t i = 0; i < size; ++i) {
        assert(get_bit(bitmap, page_frame_index + i) == true, "Double-free page frame");
    }

    set_bits(bitmap, page_frame_index, size, false);
    global_page_frame_allocator_data.free_page_frame_number += size;
}
