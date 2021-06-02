#include <debug/assert.h>
#include <interrupts/initialize.h>
#include <kernel/boot_data.h>
#include <kernel/shell.h>
#include <memory/frame_allocator.h>
#include <memory/page.h>
#include <memory/segment.h>

int _start(const struct boot_data boot_data)
{
    print_initialize(boot_data.frame_buffer_data, boot_data.psf1_data);

    int result = frame_allocator_initialize(boot_data.memory_map_data);
    assert(result == 0, "Failed to initialize the page frame allocator.");

    struct page_data kernel_page_data = { .level4_table = PAGE_NULL };
    result = page_initialize_kernel_map(&kernel_page_data);
    assert(result == 0, "Failed to initialize the page.");
    page_load(kernel_page_data);

    segment_initialize();

    interrupts_initialize();

    shell_start();

    return 0;
}
