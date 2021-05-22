;--------------------------------------------------------------------------------------------------
; Load the given global descriptor register entry into the GDTR register.
;
; This function also changes contents of segment registers according to the loaded GDT entry.
;--------------------------------------------------------------------------------------------------

; TODO: Use segment selector definitions in "global_descriptor_table.h" header.

[bits 64]

load_global_descriptor_table:
	lgdt [rdi]

	mov ax, 0x00 ; Null segment selector.
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax

	pop rdi ; Load return address into RDI register.
	mov rax, 0x08 ; Kernel code segment selector.
	push rax
	push rdi
	retfq ; Far return.

GLOBAL load_global_descriptor_table
