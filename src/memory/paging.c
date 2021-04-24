#include <debug/assert.h>

#include "page_frame_allocator.h"
#include "page_structure_entry.h"

#include "paging.h"

#define PAGE_NOT_PRESENT(table, offset) (!(table[offset] & GENERAL_PAGE_STRUCTURE_ENTRY_PRESENT))

extern struct page_frame_allocator_info global_page_frame_allocator_info;

static inline uint16_t get_level4_table_offset(uint64_t virtual_address)
{
    return (virtual_address >> 39) & 0x1FF;
}

static inline uint16_t get_level3_table_offset(uint64_t virtual_address)
{
    return (virtual_address >> 30) & 0x1FF;
}

static inline uint16_t get_level2_table_offset(uint64_t virtual_address)
{
    return (virtual_address >> 21) & 0x1FF;
}

static inline uint16_t get_level1_table_offset(uint64_t virtual_address)
{
    return (virtual_address >> 12) & 0x1FF;
}

static inline void set_next_page_structure(uint64_t *entry, uint64_t *next_page_structure)
{
    uint64_t next_page_structure_address = (uint64_t)next_page_structure;
    assert(next_page_structure_address % PAGE_SIZE == 0, "Page structure address not aligned");

    *entry &= ~GENERAL_PAGE_STRUCTURE_ENTRY_BASE_ADDRESS;
    *entry |= next_page_structure_address;
    *entry |= GENERAL_PAGE_STRUCTURE_ENTRY_PRESENT;
}

static inline uint64_t *get_next_page_structure(uint64_t entry)
{
    return (uint64_t *)(entry & GENERAL_PAGE_STRUCTURE_ENTRY_BASE_ADDRESS);
}

static uint64_t *request_new_page_structure(void)
{
    uint64_t *new_page_structure = request_page_frame();
    if (new_page_structure == PAGE_FRAME_NULL) {
        return PAGE_NULL;
    }

    uint64_t new_page_structure_address = (uint64_t)new_page_structure;
    assert(new_page_structure_address % PAGE_FRAME_SIZE == 0, "New page structure not aligned");
    assert(new_page_structure_address < GENERAL_PAGE_STRUCTURE_ENTRY_BASE_ADDRESS,
            "New page structure address out of address space");

    for (uint64_t i = 0; i < 512; ++i) {
        new_page_structure[i] = 0;
        new_page_structure[i] |= GENERAL_PAGE_STRUCTURE_ENTRY_DEFAULT_FLAG;
    }

    return new_page_structure;
}

static int set_new_page_structure(uint64_t *entry)
{
    uint64_t *new_page_structure = request_new_page_structure();
    if (new_page_structure == PAGE_NULL) {
        return 1;
    }

    set_next_page_structure(entry, new_page_structure);

    return 0;
}

int init_kernel_page_map(struct paging_info *paging_info)
{
    int result;

    if (paging_info->level4_table == PAGE_NULL) {
        void *new_page_structure = request_new_page_structure();
        if (new_page_structure == PAGE_NULL) {
            return 1;
        }

        paging_info->level4_table = new_page_structure;
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
    assert(virtual_page_address % PAGE_SIZE == 0, "Not aligned virtual address");
    assert(physical_page_address % PAGE_SIZE == 0, "Not aligned physical address");

    int result;

    uint64_t *level4_table = paging_info->level4_table;
    uint64_t level4_table_offset = get_level4_table_offset(virtual_page_address);
    if (PAGE_NOT_PRESENT(level4_table, level4_table_offset)) {
        result = set_new_page_structure(&level4_table[level4_table_offset]);
        if (result != 0) {
            return 1;
        }
    }

    uint64_t *level3_table = get_next_page_structure(level4_table[level4_table_offset]);
    uint64_t level3_table_offset = get_level3_table_offset(virtual_page_address);
    if (PAGE_NOT_PRESENT(level3_table, level3_table_offset)) {
        result = set_new_page_structure(&level3_table[level3_table_offset]);
        if (result != 0) {
            return 1;
        }
    }

    uint64_t *level2_table = get_next_page_structure(level3_table[level3_table_offset]);
    uint64_t level2_table_offset = get_level2_table_offset(virtual_page_address);
    if (PAGE_NOT_PRESENT(level2_table, level2_table_offset)) {
        result = set_new_page_structure(&level2_table[level2_table_offset]);
        if (result != 0) {
            return 1;
        }
    }

    uint64_t *level1_table = get_next_page_structure(level2_table[level2_table_offset]);
    uint64_t level1_offset = get_level1_table_offset(virtual_page_address);
    set_next_page_structure(&level1_table[level1_offset], (uint64_t *)physical_page_address);

    // TODO: Flush entries in the TLB.

    return 0;
}

void change_current_page_map(struct paging_info *paging_info)
{
    uint64_t level4_table_address = (uint64_t)paging_info->level4_table;
    assert(level4_table_address % PAGE_SIZE == 0, "Not aligned PML4");

    asm __volatile__ (
        "mov %0, %%cr3\n\t"
        :
        : "r"(level4_table_address)
        :
    );
}
