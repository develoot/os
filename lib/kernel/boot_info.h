#ifndef _KERNEL_BOOT_INFO_H
#define _KERNEL_BOOT_INFO_H

#include <stdint.h>

#include <drivers/graphic/screen.h>
#include <kernel/print.h>
#include <uefi/uefi.h>

struct kernel_boot_info {
    uint64_t kernel_start_address;
    uint64_t kernel_end_address;

    struct uefi_memory_map_info memory_map_info;
    struct graphic_frame_buffer_info frame_buffer_info;
    struct psf1_info psf1_info;
};

#endif
