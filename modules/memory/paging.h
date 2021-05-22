#ifndef _MEMORY_PAGING_H
#define _MEMORY_PAGING_H

#include <stdint.h>

#include <general/address.h>

#include "page_frame_size.h"

#define PAGE_SIZE (PAGE_FRAME_SIZE)
#define PAGE_NULL ((void *)(0xFFFFFFFFFFFFFFFF))

struct paging_data {
    uint64_t *level4_table;
};

int initialize_kernel_page_map(struct paging_data *const paging_data);

int map_page(struct paging_data *const paging_data,
        address_t virtual_address, address_t physical_address);

void change_current_page_map(struct paging_data paging_data);

#endif
