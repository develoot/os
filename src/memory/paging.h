#ifndef _MEMORY_PAGING_H
#define _MEMORY_PAGING_H

#include <stdint.h>
#include "page_frame_allocator.h"

#define PAGE_NULL (PAGE_FRAME_NULL)
#define PAGE_SIZE (PAGE_FRAME_SIZE)

struct paging_info {
    uint64_t *level4_table;
};

int init_kernel_page_map(struct paging_info *paging_info);
int map_page(struct paging_info *paging_info, uint64_t virtual_address, uint64_t physical_address);
void change_current_page_map(struct paging_info *paging_info);

#endif
