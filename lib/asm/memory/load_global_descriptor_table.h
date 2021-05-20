#ifndef _LOAD_GLOBAL_DESCRIPTOR_TABLE_H
#define _LOAD_GLOBAL_DESCRIPTOR_TABLE_H

#include <memory/segmentation/global_descriptor_table.h>

void load_global_descriptor_table(
        const struct global_descriptor_table_register_entry *register_entry);

#endif
