#include <debug/assert.h>

#include "page_table.h"
#include "paging.h"

#define PAGE_NOT_PRESENT(table, offset) \
    ((table[offset] & GENERAL_TABLE_ENTRY_PRESENT) == 0)

extern struct page_frame_allocator_info global_page_frame_allocator_info;

static inline uint16_t get_level4_offset(uint64_t virtual_address)
{
    return (virtual_address >> 39) & 0x1FF;
}

static inline uint16_t get_level3_offset(uint64_t virtual_address)
{
    return (virtual_address >> 30) & 0x1FF;
}

static inline uint16_t get_level2_offset(uint64_t virtual_address)
{
    return (virtual_address >> 21) & 0x1FF;
}

static inline uint16_t get_level1_offset(uint64_t virtual_address)
{
    return (virtual_address >> 12) & 0x1FF;
}

static inline void set_table(uint64_t *entry, uint64_t *table)
{
    uint64_t table_address = (uint64_t)table;

    assert(table_address % PAGE_SIZE == 0, "Request to set not aligned paging structure address");

    *entry &= ~GENERAL_TABLE_ENTRY_BASE_ADDRESS;
    *entry |= table_address;
    *entry |= GENERAL_TABLE_ENTRY_PRESENT;
}

static inline uint64_t *get_table(uint64_t entry)
{
    return (uint64_t *)(entry & GENERAL_TABLE_ENTRY_BASE_ADDRESS);
}

static uint64_t *request_new_table()
{
    uint64_t *new_table = request_page_frame();
    if (new_table == PAGE_FRAME_NULL) {
        return PAGE_NULL;
    }

    for (uint64_t i = 0; i < 512; ++i) {
        new_table[i] = 0;
        new_table[i] |= PAGE_DEFAULT_ATTRIBUTE;
    }

    return new_table;
}

static int add_new_table(uint64_t *entry)
{
    uint64_t *new_table = request_new_table();
    if (new_table == PAGE_NULL) {
        return 1;
    }

    set_table(entry, new_table);

    return 0;
}

int init_kernel_page_map(struct paging_info *paging_info)
{
    int result;

    if (paging_info->level4_table == PAGE_NULL) {
        void *new_table = request_new_table();
        if (new_table == PAGE_NULL) {
            return 1;
        }

        paging_info->level4_table = new_table;
    }

    for (uint64_t i = 0; i < global_page_frame_allocator_info.total_page_frame_number; ++i) {
        result = map_page(paging_info, i * PAGE_SIZE, i * PAGE_SIZE);
        if (result != 0) {
            return 1;
        }
    }

    return 0;
}

int map_page(struct paging_info *paging_info,
        uint64_t virtual_page_address, uint64_t physical_page_address)
{
    assert(virtual_page_address % PAGE_SIZE == 0, "Request to map not aligned virtual address");
    assert(physical_page_address % PAGE_SIZE == 0, "Request to map not aligned physical address");

    int result;

    uint64_t *level4_table = paging_info->level4_table;
    uint64_t level4_offset = get_level4_offset(virtual_page_address);
    if (PAGE_NOT_PRESENT(level4_table, level4_offset)) {
        result = add_new_table(&level4_table[level4_offset]);
        if (result != 0) {
            return 1;
        }
    }

    uint64_t *level3_table = get_table(level4_table[level4_offset]);
    uint64_t level3_offset = get_level3_offset(virtual_page_address);
    if (PAGE_NOT_PRESENT(level3_table, level3_offset)) {
        add_new_table(&level3_table[level3_offset]);
        if (result != 0) {
            return 1;
        }
    }

    uint64_t *level2_table = get_table(level3_table[level3_offset]);
    uint64_t level2_offset = get_level2_offset(virtual_page_address);
    if (PAGE_NOT_PRESENT(level2_table, level2_offset)) {
        add_new_table(&level2_table[level2_offset]);
        if (result != 0) {
            return 1;
        }
    }

    uint64_t *level1_table = get_table(level2_table[level2_offset]);
    uint64_t level1_offset = get_level1_offset(virtual_page_address);
    set_table(&level1_table[level1_offset], (uint64_t *)physical_page_address);

    // TODO: Flush entries in the TLB.

    return 0;
}

void change_current_page_map(struct paging_info *paging_info)
{
    uint64_t level4_table_address = (uint64_t)paging_info->level4_table;

    assert(level4_table_address % PAGE_SIZE == 0, "Request to use not aligned page map");
    assert(level4_table_address < GENERAL_TABLE_ENTRY_BASE_ADDRESS,
            "Reserved bits of table address are set to 1");

    asm __volatile__ (
        "mov %0, %%cr3\n\t"
        :
        : "r"(level4_table_address)
        :
    );
}
