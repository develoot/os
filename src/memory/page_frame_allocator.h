#ifndef _MEMORY_PAGE_FRAME_ALLOCATOR_H
#define _MEMORY_PAGE_FRAME_ALLOCATOR_H

#include <uefi/uefi.h>
#include "page_frame_allocator_type.h"

int init_page_frame_allocator(struct uefi_memory_map_info memory_map_info);
page_frame_t request_page_frame(void);
void free_page_frame(page_frame_t page_frame);

#endif
