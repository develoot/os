#ifndef _GENERAL_CIRCULAR_QUEUE_H
#define _GENERAL_CIRCULAR_QUEUE_H

#include <stdbool.h>
#include <stdint.h>

struct circular_queue_data {
    uint64_t buffer_size;
    uint64_t entry_size;
    uint64_t entry_number;
    uint64_t max_entry_number;
    uint64_t head_entry_index;
    uint64_t tail_entry_index;
    uint8_t *buffer;
};

void circular_queue_initialize(struct circular_queue_data *const circular_queue_data,
        void *const buffer, uint64_t buffer_size, uint64_t entry_size);

int circular_queue_push(struct circular_queue_data *const circular_queue_data,
        const void *entry);

int circular_queue_pop(struct circular_queue_data *const circular_queue_data,
        void *const destination);

bool circular_queue_is_empty(const struct circular_queue_data *const circular_queue_data);

#endif
