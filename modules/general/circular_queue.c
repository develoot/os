#include <general/memory_utils.h>

#include "circular_queue.h"

void initialize_circular_queue(struct circular_queue_data *const circular_queue_data,
        uint8_t *const buffer, uint64_t buffer_size, uint64_t entry_size)
{
    circular_queue_data->buffer = buffer;
    circular_queue_data->buffer_size = buffer_size;
    circular_queue_data->entry_size = entry_size;
    circular_queue_data->entry_number = 0;
    circular_queue_data->max_entry_number = buffer_size / entry_size;
    circular_queue_data->head_entry_index = 0;
    circular_queue_data->tail_entry_index = 0;
}

bool circular_queue_push(struct circular_queue_data *const circular_queue_data,
        const uint8_t *entry)
{
    uint8_t *const buffer = circular_queue_data->buffer;
    const uint64_t entry_size = circular_queue_data->entry_size;
    const uint64_t max_entry_number = circular_queue_data->max_entry_number;

    if (circular_queue_data->entry_number >= max_entry_number) {
        return true;
    }

    memory_copy(&buffer[circular_queue_data->tail_entry_index * entry_size], entry, entry_size);

    ++circular_queue_data->entry_number;
    circular_queue_data->tail_entry_index += 1;
    circular_queue_data->tail_entry_index %= max_entry_number;

    return false;
}

bool circular_queue_pop(struct circular_queue_data *const circular_queue_data,
        uint8_t *const destination)
{
    uint8_t *const buffer = circular_queue_data->buffer;
    const uint64_t entry_size = circular_queue_data->entry_size;
    const uint64_t max_entry_number = circular_queue_data->max_entry_number;

    if (circular_queue_data->entry_number <= 0) {
        return true;
    }

    memory_copy(destination,
            &buffer[circular_queue_data->head_entry_index * entry_size], entry_size);

    --circular_queue_data->entry_number;
    circular_queue_data->head_entry_index += 1;
    circular_queue_data->head_entry_index %= max_entry_number;

    return false;
}
