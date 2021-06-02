#ifndef _MEMORY_PAGE_H
#define _MEMORY_PAGE_H

#include <stdint.h>
#include <general/address.h>

#include "frame_size.h"

#define PAGE_SIZE (MEMORY_FRAME_SIZE)
#define PAGE_NULL ((void *)(0xFFFFFFFFFFFFFFFF))

struct page_data {
    uint64_t *level4_table;
};

int page_initialize_kernel_map(struct page_data *const page_data);

int page_map(struct page_data *const page_data,
        address_t virtual_address, address_t physical_address);

void page_load(struct page_data page_data);

#endif
