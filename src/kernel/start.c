#include <debug/assert.h>

#include <kernel/boot_data.h>
#include <kernel/error.h>
#include <kernel/shell.h>

#include <memory/page_frame_allocator.h>
#include <memory/paging.h>
#include <memory/segmentation/initialize.h>

#include <interrupts/initialize.h>

int _start(const struct kernel_boot_data boot_data)
{
    initialize_print(boot_data.frame_buffer_data, boot_data.psf1_data);

    int result = initialize_page_frame_allocator(boot_data.memory_map_data);
    assert(!KERNEL_ERROR(result), "Failed to initialize the page frame allocator.");

    struct paging_data kernel_paging_data = { .level4_table = PAGE_NULL };
    result = initialize_kernel_page_map(&kernel_paging_data);
    assert(!KERNEL_ERROR(result), "Failed to initialize the paging.");

    change_current_page_map(kernel_paging_data);

    initialize_segmentation();
    initialize_interrupts();

    start_shell();

    while (1) {}

    return 0;
}
