#include "linked_list.h"

void linked_list_initialize(struct linked_list_node *const head)
{
    head->next = head;
    head->previous = head;
}

void linked_list_append(struct linked_list_node *const head, struct linked_list_node *const node)
{
    struct linked_list_node *const last = head->previous;

    last->next = node;
    node->previous = last;
}

void linked_list_remove(struct linked_list_node *const node)
{
    struct linked_list_node *const previous = node->previous;
    struct linked_list_node *const next = node->next;

    previous->next = next;
    next->previous = previous;
}

bool linked_list_is_empty(struct linked_list_node *const head)
{
    return head->next == head;
}

size_t linked_list_size(struct linked_list_node *const head)
{
    size_t size = 0;
    struct linked_list_node *cursor = NULL;

    linked_list_for_each(cursor, head) {
        size++;
    }

    return size;
}
