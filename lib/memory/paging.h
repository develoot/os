#ifndef _MEMORY_PAGING_H
#define _MEMORY_PAGING_H

#include "paging_type.h"

int init_kernel_page_map(struct paging_info *paging_info);
int map_page(struct paging_info *paging_info, uint64_t virtual_address, uint64_t physical_address);
void change_current_page_map(struct paging_info *paging_info);

#endif
