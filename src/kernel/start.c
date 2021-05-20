#include <interrupts/initialize.h>
#include <kernel/boot_info.h>
#include <kernel/error.h>
#include <kernel/shell.h>
#include <memory/page_frame_allocator.h>
#include <memory/paging.h>
#include <memory/segmentation/initialize.h>

int _start(const struct kernel_boot_info boot_info)
{
    int result;

    init_print(boot_info.frame_buffer_info, boot_info.psf1_info);

    result = initialize_page_frame_allocator(boot_info.memory_map_info);
    if (KERNEL_ERROR(result)) {
        print_string("InitPFAllocError ");
        while (1) {}
    }

    struct paging_data kernel_paging_data;
    kernel_paging_data.level4_table = PAGE_NULL;
    result = initialize_kernel_page_map(&kernel_paging_data);
    if (KERNEL_ERROR(result)) {
        print_string("InitPageMapError ");
        while (1) {}
    }
    change_current_page_map(kernel_paging_data);
    initialize_segmentation();
    initialize_interrupts();

    start_shell();

    while (1) {}

    return 0;
}
