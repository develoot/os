#include <stdbool.h>
#include <debug/assert.h>

#include "frame_allocator.h"
#include "page_structure_entry.h"
#include "page.h"

static inline bool page_not_present(uint64_t *const table, const uint16_t offset)
{
    return !(table[offset] & PAGE_STRUCTURE_ENTRY_PRESENT);
}

static inline uint16_t get_level4_table_offset(address_t virtual_address)
{
    return (virtual_address >> 39) & 0x1FF;
}

static inline uint16_t get_level3_table_offset(address_t virtual_address)
{
    return (virtual_address >> 30) & 0x1FF;
}

static inline uint16_t get_level2_table_offset(address_t virtual_address)
{
    return (virtual_address >> 21) & 0x1FF;
}

static inline uint16_t get_level1_table_offset(address_t virtual_address)
{
    return (virtual_address >> 12) & 0x1FF;
}

static inline void set_next_page_structure(uint64_t *const page_table_entry,
        const uint64_t *const next_page_structure)
{
    *page_table_entry &= ~PAGE_STRUCTURE_ENTRY_BASE_ADDRESS;
    *page_table_entry |= (uint64_t)next_page_structure;
    *page_table_entry |= PAGE_STRUCTURE_ENTRY_PRESENT;
}

static inline uint64_t *get_next_page_structure(uint64_t page_table_entry)
{
    return (uint64_t *)(page_table_entry & PAGE_STRUCTURE_ENTRY_BASE_ADDRESS);
}

static uint64_t *request_new_page_structure(void)
{
    uint64_t *const new_page_structure = (uint64_t *)frame_allcoator_request(1);
    if (new_page_structure == MEMORY_FRAME_NULL) {
        return PAGE_NULL;
    }

    const address_t new_page_structure_address = (address_t)new_page_structure;
    assert(new_page_structure_address % MEMORY_FRAME_SIZE == 0, "New page structure is not aligned");
    assert(new_page_structure_address < PAGE_STRUCTURE_ENTRY_BASE_ADDRESS,
            "New page structure address out of address space");

    for (uint64_t i = 0; i < 512; ++i) {
        new_page_structure[i] = 0;
        new_page_structure[i] |= PAGE_STRUCTURE_ENTRY_DEFAULT;
    }

    return new_page_structure;
}

static int set_new_page_structure(uint64_t *page_table_entry)
{
    uint64_t *const new_page_structure = request_new_page_structure();
    if (new_page_structure == PAGE_NULL) {
        return 1;
    }

    set_next_page_structure(page_table_entry, new_page_structure);

    return 0;
}

int page_initialize_kernel_map(struct page_data *const page_data)
{
    if (page_data->level4_table == PAGE_NULL) {
        void *const new_page_structure = request_new_page_structure();
        if (new_page_structure == PAGE_NULL) {
            return 1;
        }
        page_data->level4_table = new_page_structure;
    }

    for (uint64_t i = 0; i < frame_allocator_get_total_frame_number(); ++i) {
        int result = page_map(page_data, i * PAGE_SIZE, i * PAGE_SIZE);
        if (result != 0) {
            return 1;
        }
    }

    return 0;
}

int page_map(struct page_data *const page_data,
        address_t virtual_page_address, address_t physical_page_address)
{
    assert(virtual_page_address % PAGE_SIZE == 0, "Not aligned virtual address");
    assert(physical_page_address % PAGE_SIZE == 0, "Not aligned physical address");

    int result = 0;

    uint64_t *const level4_table = page_data->level4_table;
    uint64_t level4_table_offset = get_level4_table_offset(virtual_page_address);
    if (page_not_present(level4_table, level4_table_offset)) {
        result = set_new_page_structure(&level4_table[level4_table_offset]);
        if (result != 0) {
            return 1;
        }
    }

    uint64_t *const level3_table = get_next_page_structure(level4_table[level4_table_offset]);
    uint64_t level3_table_offset = get_level3_table_offset(virtual_page_address);
    if (page_not_present(level3_table, level3_table_offset)) {
        result = set_new_page_structure(&level3_table[level3_table_offset]);
        if (result != 0) {
            return 1;
        }
    }

    uint64_t *const level2_table = get_next_page_structure(level3_table[level3_table_offset]);
    uint64_t level2_table_offset = get_level2_table_offset(virtual_page_address);
    if (page_not_present(level2_table, level2_table_offset)) {
        result = set_new_page_structure(&level2_table[level2_table_offset]);
        if (result != 0) {
            return 1;
        }
    }

    uint64_t *const level1_table = get_next_page_structure(level2_table[level2_table_offset]);
    uint64_t level1_offset = get_level1_table_offset(virtual_page_address);
    set_next_page_structure(&level1_table[level1_offset], (uint64_t *)physical_page_address);

    // TODO: Flush entries in the TLB.

    return 0;
}

void page_load(struct page_data page_data)
{
    address_t level4_table_address = (address_t)page_data.level4_table;
    assert(level4_table_address % PAGE_SIZE == 0, "Not aligned PML4");
    asm __volatile__ ("mov %0, %%cr3\n\t" : : "r"(level4_table_address));
}
