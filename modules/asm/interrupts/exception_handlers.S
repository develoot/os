;--------------------------------------------------------------------------------------------------
; Interrupt Service Routine (ISR) definitions for each exceptions.
;--------------------------------------------------------------------------------------------------

; TODO: Use segment selector definitions in "global_descriptor_table.h" header.

@macro save_context 0
	push rbp
	mov rbp rsp

	push rax
	push rbx
	push rcx
	push rdx
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	mov ax, ds
	push rax
	mov ax, es
	push rax
	push fs
	push gs

	mov ax, 0x10 ; Kernel data segment descriptor.
	mov ds, ax
	mov es, ax
	mov gs, ax
	mov fs,ax
@endmacro

@macro load_context 0
	pop gs
	pop fs
	pop rax
	mov es, ax
	pop rax
	mov ds, ax

	pop r15
	pop r14
	pop r13
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rdx
	pop rcx
	pop rbx
	pop rax
	pop rbp
@endmacro

timeout_handler:
	save_context

	mov rdi, 32
	call notify_end_of_interrupt

	load_context
	iretq

keyboard_handler:
	save_context

	mov rdi, 33
	call notify_end_of_interrupt

	load_context
	iretq
