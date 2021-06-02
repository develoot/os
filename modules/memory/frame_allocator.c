/** TODO: Implement zoned page frame allocator. */

#include <stdbool.h>

#include <debug/assert.h>
#include <general/address.h>

#include "frame_allocator.h"

#define MEMORY_FRAME_BITMAP_MAX_PAGE_NUMBER (MEMORY_FRAME_BITMAP_MAX_SIZE * 8)
#define MEMORY_FRAME_BITMAP_MAX_SIZE (32 * 1024 * 512) // 512 GB address space.

struct frame_allocator_data {
    /**
     * A bitmap that represents status of page frames.
     *
     * Each bits in this array represents status of the page frame.
     *
     * A page frame is in use if the bit is set. It's available otherwise.
     */
    uint8_t  bitmap[MEMORY_FRAME_BITMAP_MAX_SIZE];
    uint64_t bitmap_size;
    uint64_t total_frame_number;
    uint64_t free_frame_number;
};

static struct frame_allocator_data global_frame_allocator_data;

static inline uint64_t convert_address_to_index(address_t frame_address)
{
    assert(frame_address % MEMORY_FRAME_SIZE == 0, "Not aligned page frame");
    return frame_address / MEMORY_FRAME_SIZE;
}

static inline address_t convert_index_to_address(uint64_t frame_index)
{
    assert(frame_index < MEMORY_FRAME_BITMAP_MAX_PAGE_NUMBER, "Page frame index too large");
    return frame_index * MEMORY_FRAME_SIZE;
}

static inline void set_bit(uint8_t *const bitmap, uint64_t bit_index, bool value)
{
    bitmap[bit_index / 8] &= ~(1 << (bit_index % 8));
    bitmap[bit_index / 8] |= (value << (bit_index % 8));
}

static inline void set_bits(uint8_t *const bitmap, uint64_t bit_index, uint64_t size,
        bool value)
{
    for (uint64_t i = 0; i < size; ++i) {
        set_bit(bitmap, bit_index + i, value);
    }
}

static inline bool get_bit(uint8_t *const bitmap, uint64_t bit_index)
{
    return (bitmap[bit_index / 8] & (1 << (bit_index % 8))) > 0;
}

static uint64_t get_total_uefi_frame_number(struct uefi_memory_map_data memory_map_data)
{
    uint64_t total_uefi_frame_number = 0;

    uefi_memory_descriptor_for_each(
            descriptor,
            memory_map_data.memory_descriptor_buffer,
            memory_map_data.memory_descriptor_buffer_size,
            memory_map_data.memory_descriptor_size) {
        total_uefi_frame_number += descriptor->NumberOfPages;
    }

    return total_uefi_frame_number;
}

static int initialize_frame_bitmap(uint8_t *bitmap,
        struct uefi_memory_map_data memory_map_data)
{
    uefi_memory_descriptor_for_each(
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

int frame_allocator_initialize(struct uefi_memory_map_data memory_map_data)
{
    uint64_t total_uefi_frame_number = get_total_uefi_frame_number(memory_map_data);
    if (total_uefi_frame_number <= 0) {
        return 1;
    }
    if (total_uefi_frame_number > MEMORY_FRAME_BITMAP_MAX_PAGE_NUMBER) {
        total_uefi_frame_number = MEMORY_FRAME_BITMAP_MAX_PAGE_NUMBER;
    }

    global_frame_allocator_data.bitmap_size = ((total_uefi_frame_number - 1) / 8) + 1;

    for (uint64_t i = 0; i < global_frame_allocator_data.bitmap_size; ++i) {
        global_frame_allocator_data.bitmap[i] = 0xFF;
    }

    global_frame_allocator_data.total_frame_number = total_uefi_frame_number;
    global_frame_allocator_data.free_frame_number  = total_uefi_frame_number;

    int result = initialize_frame_bitmap(global_frame_allocator_data.bitmap,
            memory_map_data);
    if (result != 0) {
        return 1;
    }

    return 0;
}

uint64_t frame_allocator_get_total_frame_number(void)
{
    return global_frame_allocator_data.total_frame_number;
}

frame_t frame_allcoator_request(uint64_t size)
{
    if (global_frame_allocator_data.free_frame_number < size) {
        return MEMORY_FRAME_NULL;
    }

    frame_t new_frame = MEMORY_FRAME_NULL;
    uint8_t *const bitmap = global_frame_allocator_data.bitmap;

    uint64_t current_frame_size = 0;
    for (uint64_t bit_index = 0;
            bit_index < global_frame_allocator_data.total_frame_number;
            bit_index++) {
        if (get_bit(bitmap, bit_index) == true) {
            current_frame_size = 0;
            continue;
        }
        current_frame_size++;
        if (current_frame_size == size) {
            set_bits(bitmap, bit_index - size + 1, size, true);
            global_frame_allocator_data.free_frame_number -= size;
            new_frame = (frame_t)convert_index_to_address(bit_index - size + 1);
            break;
        }
    }

    return new_frame;
}

void frame_allocator_free(frame_t frame, uint64_t size)
{
    address_t frame_address = (address_t)frame;
    assert(frame_address % MEMORY_FRAME_SIZE == 0, "Not aligned page frame");

    uint8_t *const bitmap = global_frame_allocator_data.bitmap;
    uint64_t frame_index = convert_address_to_index(frame_address);
    for (uint64_t i = 0; i < size; ++i) {
        assert(get_bit(bitmap, frame_index + i) == true, "Double-free page frame");
    }

    set_bits(bitmap, frame_index, size, false);
    global_frame_allocator_data.free_frame_number += size;
}
