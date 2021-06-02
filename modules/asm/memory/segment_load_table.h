#ifndef _ASM_MEMORY_SEGMENT_LOAD_TABLE_H
#define _ASM_MEMORY_SEGMENT_LOAD_TABLE_H

#include <memory/global_descriptor_table.h>

void segment_load_table(const struct global_descriptor_table_register_entry *register_entry);

#endif
