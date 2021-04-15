#ifndef _MEMORY_PAGE_FRAME_ALLOCATOR_TYPE_H
#define _MEMORY_PAGE_FRAME_ALLOCATOR_TYPE_H

#include <stdint.h>

#define PAGE_FRAME_BITMAP_MAX_PAGE_NUMBER (PAGE_FRAME_BITMAP_MAX_SIZE * 8)
#define PAGE_FRAME_BITMAP_MAX_SIZE (32 * 1024 * 512) // 512GB address space.
#define PAGE_FRAME_NULL ((void *)(0xFFFFFFFFFFFFFFFF))
#define PAGE_FRAME_SIZE (0x1000)

struct page_frame_allocator_info {
    /**
     * A bitmap that represents availability of page frames.
     *
     * Each bit in this array represents availability of page frame.
     * A page frame is in use if bit is set. It's free otherwise.
     */
    uint8_t  bitmap[PAGE_FRAME_BITMAP_MAX_SIZE];
    uint64_t bitmap_size;
    uint64_t total_page_frame_number;
    uint64_t free_page_frame_number;
};

typedef void *page_frame_t;

#endif
