#ifndef _MEMORY_PAGE_FRAME_ALLOCATOR_H
#define _MEMORY_PAGE_FRAME_ALLOCATOR_H

#include <uefi/uefi.h>

#include "page_frame_size.h"

#define PAGE_FRAME_NULL ((page_frame_t)(0xFFFFFFFFFFFFFFFF))

typedef void *page_frame_t;

int initialize_page_frame_allocator(struct uefi_memory_map_info memory_map_info);
uint64_t get_total_page_frame_number(void);
/**
 * Return `size` numbers of continuous page frame.
 *
 * @return On success, start address of the requested page frame. `PAGE_FRAME_NULL` otherwise.
 */
page_frame_t request_page_frames(uint64_t size);
void free_page_frames(page_frame_t page_frame, uint64_t size);

#endif
