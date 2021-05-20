#ifndef _UEFI_H
#define _UEFI_H

#include <efi.h>

#define NEXT_DESCRIPTOR(ptr, size) \
    ((EFI_MEMORY_DESCRIPTOR *)((uint8_t *)ptr + size))

#define FOR_EACH_DESCRIPTOR(name, buffer, buffer_size, descriptor_size) \
    for (EFI_MEMORY_DESCRIPTOR *name = buffer; \
            (uint8_t *)name < (uint8_t *)buffer + buffer_size; \
            name = NEXT_DESCRIPTOR(name, descriptor_size))

struct uefi_memory_map_data {
    EFI_MEMORY_DESCRIPTOR *memory_descriptor_buffer;
    UINTN memory_descriptor_buffer_size;
    UINT32 memory_descriptor_version;
    UINTN memory_descriptor_size;
    UINTN memory_map_key;
};

#endif
