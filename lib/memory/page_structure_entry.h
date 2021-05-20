#ifndef _MEMORY_PAGE_STRUCTURE_ENTRY_H
#define _MEMORY_PAGE_STRUCTURE_ENTRY_H

/**
 * Bit-mask definitions of entries of the memory map page structures.
 *
 * Bit fields not covered here are reserved or unused bits.
 */

/**
 * Present flag.
 *
 * The processor seeks next paging structure if this bit is set.
 *
 * Otherwise, the processor generates page fault exception.
 */
#define GENERAL_PAGE_STRUCTURE_ENTRY_PRESENT (0x0000000000000001)
/**
 * Read/write flag.
 *
 * If unset, writes are not be allowed to the region controlled by this entry.
 */
#define GENERAL_PAGE_STRUCTURE_ENTRY_READ_WRITE (0x0000000000000002)
/**
 * User/supervisor flag.
 *
 * If unset, user-mode accesses are not allowed to the region controlled by this entry.
 */
#define GENERAL_PAGE_STRUCTURE_ENTRY_USER_SUPER (0x0000000000000004)
/**
 * Page-level write-through flag.
 *
 * Indirectly determines the memory type used to access the next paging structure referenced by this
 * entry.
 *
 * @see Section 4.5, Intel 64 and IA-32 architectures software developer's manual
 */
#define GENERAL_PAGE_STRUCTURE_ENTRY_CACHE_WRITE_THROUGH (0x0000000000000008)
/**
 * Page-level cache disabled flag.
 *
 * Indirectly determines the memory type used to access the next paging structure referenced by this
 * entry.
 *
 * @see Section 4.5, Intel 64 and IA-32 architectures software developer's manual
 */
#define GENERAL_PAGE_STRUCTURE_ENTRY_CACHE_DISABLED (0x0000000000000010)
/**
 * Accessed flag.
 *
 * Indicates whether this entry has been used for linear-address translation.
 */
#define GENERAL_PAGE_STRUCTURE_ENTRY_ACCESSED (0x0000000000000020)
/**
 * Dirty flag.
 *
 * Indicates whether software has written to the 4KB page referenced by this entry.
 */
#define PAGE_TABLE_ENTRY_DIRTY (0x0000000000000040)
/**
 * Page size flag.
 *
 * Must be unset. otherwise, this entry dose not maps to 4KB page.
 */
#define GENERAL_PAGE_STRUCTURE_ENTRY_PAGE_SIZE (0x0000000000000080)
/**
 * Page access yype flag.
 *
 * Indirectly dertermines the memory type used to access the 4KB page referenced by this entry.
 */
#define PAGE_TABLE_ENTRY_ACCESS_TYPE (0x0000000000000080)
/**
 * Global flag.
 *
 * If CR4.PGE = 1, determines whether the translation is global. Ignored otherwise.
 */
#define PAGE_TABLE_ENTRY_GLOBAL (0x0000000000000100)
/**
 * Physical address of next paging structure referenced by this entry.
 *
 * Note that actual size of field that represents physical address varies based on the
 * implementation of processor.
 *
 * The field 51:M where M is abbreviation for the highest bit of the address field are reserved
 * so that they must be unset. If those bits are not unset, page fault exception occurs.
 *
 * The exact value of M can be found by using CPUID instruction. But, we just let M 48 for now.
 */
#define GENERAL_PAGE_STRUCTURE_ENTRY_BASE_ADDRESS          (0x000FFFFFFFFFF000)
#define GENERAL_PAGE_STRUCTURE_ENTRY_BASE_ADDRESS_RESERVED (0x000F000000000000)
/**
 * Execution-disable flag.
 *
 * If IA32_EFER.NXE = 1, disable execution on the region controlled by this structure.
 *
 * If set, instruction fetches are not allowed from the region controlled by this entry.
 * Reserved otherwise.
 */
#define GENERAL_TABLE_ENTRY_EXECUTION_DISABLED (0x8000000000000000)

#define GENERAL_PAGE_STRUCTURE_ENTRY_DEFAULT_FLAG (GENERAL_PAGE_STRUCTURE_ENTRY_READ_WRITE)

#endif
