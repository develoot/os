;--------------------------------------------------------------------------------------------------
; Load the given global descriptor register entry into the GDTR register.
;
; This function also changes contents of segment registers according to the loaded GDT entry.
;--------------------------------------------------------------------------------------------------

[bits 64]

global load_global_descriptor_table

section .text

load_global_descriptor_table:
	lgdt [rdi]

	mov ax, 0x10 ; Kernel data segment selector.
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
