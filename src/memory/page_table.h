#ifndef _MEMORY_PAGE_TABLE_H
#define _MEMORY_PAGE_TABLE_H

/**
 * Bit definitions of memory page map entries.
 *
 * Bits not defined here are reserved or unused bits.
 * As a general rule, set those bits 0 and do not tuch them.
 */

/**
 * Present; must be 1 to reference next paging structure.
 */
#define GENERAL_TABLE_ENTRY_PRESENT (0x0000000000000001)
/**
 * Read/write; if 0, writes may not be allowed to the region controlled by this entry.
 */
#define GENERAL_TABLE_ENTRY_READ_WRITE (0x0000000000000002)
/**
 * User/supervisor; if 0, user-mode accesses are not allowed to the region controlled by this entry.
 */
#define GENERAL_TABLE_ENTRY_USER_SUPER (0x0000000000000004)
/**
 * Page-level write-through; indirectly determines the memory type used to access the
 * next paging structure referenced by this entry.
 */
#define GENERAL_TABLE_ENTRY_CACHE_WRITE_THROUGH (0x0000000000000008)
/**
 * Page-level cache disabled; indirectly determines the memory type used to access the
 * next paging structure referenced by this entry.
 */
#define GENERAL_TABLE_ENTRY_CACHE_DISABLED (0x0000000000000010)
/**
 * Accessed; indicates whether this entry has been used for linear-address translation.
 */
#define GENERAL_TABLE_ENTRY_ACCESSED (0x0000000000000020)
/**
 * Dirty; indicates whether software has written to the 4KB pae referenced by this entry.
 */
#define PAGE_TABLE_ENTRY_DIRTY (0x0000000000000040)
/**
 * Page size; must be 0. otherwise, this entry dose not maps to 4KB page.
 */
#define GENERAL_TABLE_ENTRY_PAGE_SIZE (0x0000000000000080) // 0: 4KB page, 1: 2MB page
/**
 * Indirectly dertermines the memory type used to access the 4KB page referenced by this entry.
 */
#define PAGE_TABLE_ENTRY_ACCESS_TYPE (0x0000000000000080)
/**
 * Global; if CR4.PGE = 1, determines whether the translation is global; ignore otherwise.
 */
#define PAGE_TABLE_ENTRY_GLOBAL (0x0000000000000100)
/**
 * Physical address of next paging structure referenced by this entry.
 *
 * Note that actual number of bits that represents physical address differs based on the
 * implementation of processor.
 *
 * The bits between [51, M] where M is abbreviation for the highest bit that represents physical
 * address are reserved bit which have to be 0. If those bits are not 0, page fault will occur.
 */
#define GENERAL_TABLE_ENTRY_BASE_ADDRESS (0x0FFFFFFFFFFFF000)
/**
 * If IA32_EFER.NXE = 1, execute-disable. if 1, instruction fetches are not allowed from the region
 * controlled by this entry; otherwise, reserved (must be 0)
 */
#define GENERAL_TABLE_ENTRY_EXECUTION_DISABLED (0x8000000000000000)

#define PAGE_DEFAULT_ATTRIBUTE (GENERAL_TABLE_ENTRY_READ_WRITE)

#endif
