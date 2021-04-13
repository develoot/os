#include <stdint.h>

#include <efi.h>
#include <efilib.h>
#include <elf.h>

#include <kernel/boot_info.h>
#include <uefi/uefi.h>

#define UEFI_MEMORY_DESCRIPTOR_BUFFER_SIZE (512)
#define PROGRAM_HEADER_TABLE_BUFFER_SIZE   (512)

EFI_MEMORY_DESCRIPTOR descriptor_buffer[UEFI_MEMORY_DESCRIPTOR_BUFFER_SIZE];
Elf64_Phdr program_header_table[PROGRAM_HEADER_TABLE_BUFFER_SIZE];

static inline int strcmp(const char *const a, const char *const b, uint64_t size)
{
    for (uint64_t i = 0; i < size; ++i) {
        if (a[i] != b[i]) {
            return 1;
        }
    }

    return 0;
}

#ifdef DEBUG_BOOT
static void print_memory_map(EFI_MEMORY_DESCRIPTOR *descriptor_buffer, UINTN descriptor_buffer_size,
        UINTN descriptor_size)
{
    static char *memory_types[] = {
        "EfiReservedMemoryType",
        "EfiLoaderCode",
        "EfiLoaderData",
        "EfiBootServicesCode",
        "EfiBootServicesData",
        "EfiRuntimeServicesCode",
        "EfiRuntimeServicesData",
        "EfiConventionalMemory",
        "EfiUnusableMemory",
        "EfiACPIReclaimMemory",
        "EfiACPIMemoryNVS",
        "EfiMemoryMappedIO",
        "EfiMemoryMappedIOPortSpace",
        "EfiPalCode"
    };

    uint64_t total_page_frame_number = 0;

    FOR_EACH_DESCRIPTOR(d, descriptor_buffer, descriptor_buffer_size, descriptor_size) {
        Print(L"%a, %u KB\n", memory_types[d->Type], d->NumberOfPages * 4);
        total_page_frame_number += d->NumberOfPages;
    }

    Print(L"Total: %u KB\n", total_page_frame_number);
}

static EFI_STATUS print_graphic_modes(EFI_GRAPHICS_OUTPUT_PROTOCOL *graphics_output)
{
    EFI_STATUS status;

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = NULL;
    uint64_t info_size = 0;

    for (uint64_t i = 0; i < graphics_output->Mode->MaxMode; ++i) {
        status = uefi_call_wrapper(graphics_output->QueryMode, 4, graphics_output, i,
                &info_size, &info);
        if (EFI_ERROR(status)) {
            return status;
        }

        Print(L"Mode: %03d, Width: %d, Height: %d, Format: %x %s\n",
                i,
                info->HorizontalResolution,
                info->VerticalResolution, info->PixelFormat,
                i == graphics_output->Mode->Mode ? "(current)" : "");
    }

    return EFI_SUCCESS;
}
#endif

EFI_STATUS open_file(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *simple_file_system, EFI_FILE **out,
        EFI_FILE *root, CHAR16 *path)
{
    EFI_STATUS status;

    if (simple_file_system == NULL) {
        return EFI_ABORTED;
    }

    if (root == NULL) {
        status = uefi_call_wrapper(simple_file_system->OpenVolume, 2, simple_file_system, &root);
        if (EFI_ERROR(status)) {
            return status;
        }
    }

    status = uefi_call_wrapper(root->Open, 5, root, out, path, EFI_FILE_MODE_READ,
            EFI_FILE_READ_ONLY);
    if (EFI_ERROR(status)) {
        return status;
    }

    return EFI_SUCCESS;
}

EFI_STATUS get_graphic_frame_buffer_info(EFI_GRAPHICS_OUTPUT_PROTOCOL *graphics_output,
        struct kernel_boot_info *boot_info)
{
    if (graphics_output->Mode->Info->PixelFormat != PixelRedGreenBlueReserved8BitPerColor
            && graphics_output->Mode->Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
        return EFI_ABORTED;
    }

#ifdef DEBUG_BOOT
    print_graphic_modes(graphics_output);
#endif

    struct graphic_frame_buffer_info *buffer_info = &boot_info->frame_buffer_info;

    buffer_info->address            = graphics_output->Mode->FrameBufferBase;
    buffer_info->size               = graphics_output->Mode->FrameBufferSize;
    buffer_info->width              = graphics_output->Mode->Info->HorizontalResolution;
    buffer_info->height             = graphics_output->Mode->Info->VerticalResolution;
    buffer_info->pixel_format       = graphics_output->Mode->Info->PixelFormat;
    buffer_info->pixel_per_scanline = graphics_output->Mode->Info->PixelsPerScanLine;

    return EFI_SUCCESS;
}

EFI_STATUS load_psf1_font(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *simple_file_system,
        struct kernel_boot_info *boot_info, EFI_FILE_PROTOCOL *root, CHAR16 *path)
{
    EFI_STATUS status;

    EFI_FILE_PROTOCOL *font_file = NULL;
    status = open_file(simple_file_system, &font_file, root, path);
    if (EFI_ERROR(status)) {
        return status;
    }

    uint64_t header_size = sizeof(boot_info->psf1_info.header);
    uefi_call_wrapper(font_file->Read, 3, font_file, &header_size, &boot_info->psf1_info.header);
    if (boot_info->psf1_info.header.magic[0] != PSF1_MAGIC0
            && boot_info->psf1_info.header.magic[1] != PSF1_MAGIC1) {
        return EFI_ABORTED;
    }

    uint64_t glyph_bufffer_size = 0;
    switch (boot_info->psf1_info.header.mode) {
    case PSF1_MODE256:
    case PSF1_MODEHASTAB:
        glyph_bufffer_size = boot_info->psf1_info.header.glyph_size * 256;
        break;
    case PSF1_MODE512:
    case PSF1_MODEHASSEQ:
    default:
        return EFI_ABORTED;
    }

    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, glyph_bufffer_size,
            (void **)&boot_info->psf1_info.glyph_buffer);
    if (EFI_ERROR(status)) {
        return status;
    }

    uefi_call_wrapper(font_file->SetPosition, 2, font_file, header_size);
    uefi_call_wrapper(font_file->Read, 3, font_file, &glyph_bufffer_size,
            boot_info->psf1_info.glyph_buffer);

    return EFI_SUCCESS;
}

EFI_STATUS load_elf_kernel(struct kernel_boot_info *boot_info, EFI_FILE_PROTOCOL *kernel_file)
{
    EFI_STATUS status;

    Elf64_Ehdr elf_header;
    uint64_t elf_header_size = sizeof(elf_header);
    uefi_call_wrapper(kernel_file->Read, 3, kernel_file, &elf_header_size, &elf_header);

    if (strcmp((char *)elf_header.e_ident, ELFMAG, SELFMAG) ||
            elf_header.e_ident[EI_CLASS]   != ELFCLASS64    ||
            elf_header.e_ident[EI_DATA]    != ELFDATA2LSB   ||
            elf_header.e_ident[EI_VERSION] != EV_CURRENT    ||
            elf_header.e_type              != ET_EXEC       ||
            elf_header.e_machine           != EM_X86_64     ||
            elf_header.e_version           != EV_CURRENT)
    {
        return EFI_ABORTED;
    }

    uint64_t program_header_table_size = elf_header.e_phentsize * elf_header.e_phnum;
    uefi_call_wrapper(kernel_file->SetPosition, 2, kernel_file, elf_header.e_phoff);
    uefi_call_wrapper(kernel_file->Read, 3, kernel_file, &program_header_table_size,
            program_header_table);

    uint64_t total_kernel_memory_size = 0;
    for (uint64_t i = 0; i < elf_header.e_phnum; ++i) {
        if (program_header_table[i].p_type == PT_LOAD) {
            total_kernel_memory_size += program_header_table[i].p_memsz;
        }
    }

    uint64_t number_of_pages =
        total_kernel_memory_size % EFI_PAGE_SIZE > 0
            ? (total_kernel_memory_size / EFI_PAGE_SIZE) + 1
            : total_kernel_memory_size / EFI_PAGE_SIZE;

    status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData,
            number_of_pages, (void **)&boot_info->kernel_start_address);
    if (EFI_ERROR(status)) {
        return status;
    }

    uint64_t kernel_total_code_size = 0;
    uint64_t segment_size = 0;

    for (uint64_t i = 0; i < elf_header.e_phnum; ++i) {
        if (program_header_table[i].p_type != PT_LOAD) {
            continue;
        }

        segment_size = program_header_table[i].p_filesz;

        uefi_call_wrapper(kernel_file->SetPosition, 2, kernel_file,
                program_header_table[i].p_offset);
        uefi_call_wrapper(kernel_file->Read, 3, kernel_file, &segment_size,
                (void *)(boot_info->kernel_start_address + kernel_total_code_size));

        if (segment_size != program_header_table[i].p_filesz) {
            return EFI_ABORTED;
        }

        kernel_total_code_size += segment_size;
    }

    boot_info->kernel_end_address = boot_info->kernel_start_address + total_kernel_memory_size;

    return 0;
}

EFI_STATUS get_memory_map_info(struct kernel_boot_info *boot_info,
        EFI_MEMORY_DESCRIPTOR *descriptor_buffer)
{
    EFI_STATUS status;

    boot_info->memory_map_info.memory_descriptor_buffer = descriptor_buffer;
    boot_info->memory_map_info.memory_descriptor_buffer_size =
                        sizeof(EFI_MEMORY_DESCRIPTOR) * UEFI_MEMORY_DESCRIPTOR_BUFFER_SIZE;

    status = uefi_call_wrapper(BS->GetMemoryMap, 5,
            &boot_info->memory_map_info.memory_descriptor_buffer_size,
            boot_info->memory_map_info.memory_descriptor_buffer,
            &boot_info->memory_map_info.memory_map_key,
            &boot_info->memory_map_info.memory_descriptor_size,
            &boot_info->memory_map_info.memory_descriptor_version);
    if (EFI_ERROR(status)) {
        return status;
    }

    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
    EFI_STATUS status;
    InitializeLib(image_handle, system_table);

    struct kernel_boot_info boot_info;

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *simple_file_system = NULL;
    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiSimpleFileSystemProtocolGuid, NULL,
            (void **)&simple_file_system);
    if (EFI_ERROR(status)) {
        Print(L"Failed to locate SimpleFileSystemProtocol. %r\n", status);
        goto ERROR;
    }

    EFI_GRAPHICS_OUTPUT_PROTOCOL *graphics_output = NULL;
    status = uefi_call_wrapper(BS->LocateProtocol, 3,
            &gEfiGraphicsOutputProtocolGuid, NULL, (void **)&graphics_output);
    if (EFI_ERROR(status)) {
        Print(L"Failed to locate GraphicsOutputProtocol. %r\n", status);
        goto ERROR;
    }

    Print(L"Open kernel file.\n");

    EFI_FILE *kernel_file = NULL;
    status = open_file(simple_file_system, &kernel_file, NULL, L"kernel.elf");
    if (EFI_ERROR(status)) {
        Print(L"Failed to open kernel file. %r\n", status);
        goto ERROR;
    }

    Print(L"Load kernel ELF file.\n");

    status = load_elf_kernel(&boot_info, kernel_file);
    if (EFI_ERROR(status)) {
        Print(L"Failed to load kernel ELF file. %r\n", status);
        goto ERROR;
    }

    Print(L"Kernel Info:\n");
    Print(L"StartAddress: 0x%X, EndAddress: 0x%X\n",
            boot_info.kernel_start_address, boot_info.kernel_end_address);

    Print(L"Get graphic frame buffer info.\n");

    status = get_graphic_frame_buffer_info(graphics_output, &boot_info);
    if (EFI_ERROR(status)) {
        Print(L"Failed to get graphic frame buffer information. %r\n", status);
        goto ERROR;
    }

    Print(L"Graphic Frame Buffer Info:\n");
    Print(L"BaseAddress: 0x%X, BufferSize: %u, Width: %u, Height: %u\n",
            boot_info.frame_buffer_info.address,
            boot_info.frame_buffer_info.size,
            boot_info.frame_buffer_info.width,
            boot_info.frame_buffer_info.height);

    Print(L"Load PSF1 font.\n");

    status = load_psf1_font(simple_file_system, &boot_info, NULL, L"zap-light16.psf");
    if (EFI_ERROR(status)) {
        Print(L"Failed to load PSF1 font. %r\n", status);
        goto ERROR;
    }

    Print(L"PSF1 Font Info:\n");
    Print(L"GlyphSize: %d\n", boot_info.psf1_info.header.glyph_size);

    Print(L"Get memory map of UEFI system.\n");

    status = get_memory_map_info(&boot_info, descriptor_buffer);
    if (EFI_ERROR(status)) {
        Print(L"Failed to get memory map of UEFI system. %r\n", status);
        goto ERROR;
    }

    Print(L"Exit boot services.\n");

    uefi_call_wrapper(BS->ExitBootServices, 2,
            image_handle, boot_info.memory_map_info.memory_map_key);

#ifdef DEBUG_BOOT
    print_memory_map(boot_info.memory_map_info.memory_descriptor_buffer,
            boot_info.memory_map_info.memory_descriptor_buffer_size,
            boot_info.memory_map_info.memory_descriptor_size);
#endif

    Print(L"Start kernel.\n");

    int (*start_kernel)(struct kernel_boot_info *) =
        (__attribute__((sysv_abi)) int (*) (struct kernel_boot_info *))
            (boot_info.kernel_start_address);

    Print(L"%d\n", start_kernel(&boot_info));

    return EFI_SUCCESS;

ERROR:
    uefi_call_wrapper(BS->Exit, 4, image_handle, EFI_ABORTED, 0, NULL);

    return EFI_ABORTED;
}
