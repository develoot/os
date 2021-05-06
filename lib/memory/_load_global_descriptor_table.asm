;--------------------------------------------------------------------------------------------------
; Load given global descriptor to GDTR register.
;
; This function also changes contents of segment registers according to the loaded GDT entry.
;--------------------------------------------------------------------------------------------------

[bits 64]

_load_global_descriptor_table:
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

GLOBAL _load_global_descriptor_table
