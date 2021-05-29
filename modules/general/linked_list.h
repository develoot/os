#ifndef _GENERAL_LINKED_LIST_H
#define _GENERAL_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <general/address.h>
#include <general/inline.h>

#define container_of(MemberAddress, ContainerType, MemberName) \
    ((ContainerType *)((address_t)(MemberAddress) - offsetof(ContainerType, MemberName)))

#define linked_list_for_each(Cursor, Head) \
    for (Cursor = (Head)->next; Cursor != (Head); Cursor = Cursor->next)

struct linked_list_node {
    struct linked_list_node *next;
    struct linked_list_node *previous;
};

always_inline void linked_list_initialize(struct linked_list_node *const head)
{
    head->next = head;
    head->previous = head;
}

always_inline void linked_list_append(struct linked_list_node *const head,
        struct linked_list_node *const node)
{
    struct linked_list_node *const last = head->previous;

    last->next = node;
    node->previous = last;
}

always_inline void linked_list_remove(struct linked_list_node *const node)
{
    struct linked_list_node *const previous = node->previous;
    struct linked_list_node *const next = node->next;

    previous->next = next;
    next->previous = previous;
}

always_inline bool linked_list_is_empty(struct linked_list_node *const head)
{
    return head->next == head;
}

always_inline size_t linked_list_size(struct linked_list_node *const head)
{
    size_t size = 0;
    struct linked_list_node *cursor = NULL;

    linked_list_for_each(cursor, head) {
        size++;
    }

    return size;
}

#endif
