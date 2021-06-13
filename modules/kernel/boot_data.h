#ifndef _KERNEL_BOOT_DATA_H
#define _KERNEL_BOOT_DATA_H

#include <stdint.h>
#include <drivers/graphic/screen.h>
#include <general/address.h>
#include <kernel/console.h>
#include <uefi/uefi.h>

struct boot_data {
    address_t kernel_start_address;
    address_t kernel_end_address;
    struct uefi_memory_map_data memory_map_data;
    struct graphic_frame_buffer_data frame_buffer_data;
    struct psf1_data psf1_data;
};

#endif
