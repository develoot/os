#ifndef _MEMORY_PAGING_TYPE_H
#define _MEMORY_PAGING_TYPE_H

#include <stdint.h>
#include "page_frame_size.h"

#define PAGE_NULL (PAGE_FRAME_NULL)
#define PAGE_SIZE (PAGE_FRAME_SIZE)

struct paging_info {
    uint64_t *level4_table;
};

#endif
