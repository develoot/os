#ifndef _MEMORY_PAGE_FRAME_ALLOCATOR_H
#define _MEMORY_PAGE_FRAME_ALLOCATOR_H

#include <uefi/uefi.h>
#include "page_frame_allocator_type.h"

int init_page_frame_allocator(struct uefi_memory_map_info memory_map_info);
uint64_t get_total_page_frame_number(void);
/**
 * Return `size` numbers of continuous page frame.
 *
 * @return Start address of requested page frame on success. `PAGE_FRAME_NULL` otherwise.
 */
page_frame_t request_page_frame(uint64_t size);
void free_page_frame(page_frame_t page_frame);

#endif
