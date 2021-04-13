#ifndef _MEMORY_PAGE_FRAME_ALLOCATOR_H
#define _MEMORY_PAGE_FRAME_ALLOCATOR_H

#include <stdint.h>

#include <uefi/uefi.h>

#define PAGE_FRAME_NULL ((void *)(0xFFFFFFFFFFFFFFFF))
#define PAGE_FRAME_SIZE (0x1000)
#define PAGE_FRAME_BITMAP_MAX_SIZE (32 * 1024 * 512) // 512GB address space.
#define PAGE_FRAME_BITMAP_MAX_PAGE_NUMBER (PAGE_FRAME_BITMAP_MAX_SIZE * 8)

typedef void *page_frame_t;

struct page_frame_allocator_info {
    /**
     * A bitmap that represents availability of page frames.
     *
     * Each bit in this array represents availability of page frame.
     * A page frame is in use if bit that represents it is 1. If it's 0, it's free.
     */
    uint8_t  bitmap[PAGE_FRAME_BITMAP_MAX_SIZE];
    uint64_t bitmap_size;
    uint64_t total_page_frame_number;
    uint64_t free_page_frame_number;
};

int init_page_frame_allocator(struct uefi_memory_map_info memory_map_info);
page_frame_t request_page_frame();
void free_page_frame(page_frame_t page_frame);

#endif
