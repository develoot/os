#ifndef _UEFI_H
#define _UEFI_H

#include <efi.h>
#include <stddef.h>
#include <general/address.h>

static inline EFI_MEMORY_DESCRIPTOR *uefi_memory_descriptor_next(
        EFI_MEMORY_DESCRIPTOR *descriptor,
        size_t size)
{
    return (EFI_MEMORY_DESCRIPTOR *)((address_t)descriptor + size);
}

#define uefi_memory_descriptor_for_each(Name, Buffer, BufferSize, DescriptorSize)   \
    for (EFI_MEMORY_DESCRIPTOR *Name = Buffer;                                      \
            (address_t)Name < (address_t)Buffer + BufferSize;                       \
            Name = uefi_memory_descriptor_next(Name, DescriptorSize))

struct uefi_memory_map_data {
    EFI_MEMORY_DESCRIPTOR *memory_descriptor_buffer;
    UINTN memory_descriptor_buffer_size;
    UINT32 memory_descriptor_version;
    UINTN memory_descriptor_size;
    UINTN memory_map_key;
};

#endif
