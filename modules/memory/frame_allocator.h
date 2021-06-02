#ifndef _MEMORY_FRAME_ALLOCATOR_H
#define _MEMORY_FRAME_ALLOCATOR_H

#include <uefi/uefi.h>

#include "frame_size.h"

#define MEMORY_FRAME_NULL ((frame_t)(0xFFFFFFFFFFFFFFFF))

typedef void *frame_t;

int frame_allocator_initialize(struct uefi_memory_map_data memory_map_data);

uint64_t frame_allocator_get_total_frame_number(void);

/**
 * Return `size` numbers of continuous page frame.
 *
 * @return On success, start address of the requested page frame. `MEMORY_FRAME_NULL` otherwise.
 */
frame_t frame_allcoator_request(uint64_t size);

void frame_allocator_free(frame_t frame, uint64_t size);

#endif
