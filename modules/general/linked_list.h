#ifndef _GENERAL_LINKED_LIST_H
#define _GENERAL_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <general/address.h>

#define container_of(MemberAddress, ContainerType, MemberName) \
    ((ContainerType *)((address_t)(MemberAddress) - offsetof(ContainerType, MemberName)))

#define linked_list_for_each(Cursor, Head) \
    for (Cursor = (Head)->next; Cursor != (Head); Cursor = Cursor->next)

struct linked_list_node {
    struct linked_list_node *next;
    struct linked_list_node *previous;
};

void linked_list_initialize(struct linked_list_node *const head);

void linked_list_append(struct linked_list_node *const head, struct linked_list_node *const node);

void linked_list_remove(struct linked_list_node *const node);

bool linked_list_is_empty(struct linked_list_node *const head);

size_t linked_list_size(struct linked_list_node *const head);

#endif
