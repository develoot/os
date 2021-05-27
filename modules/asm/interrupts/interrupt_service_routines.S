;--------------------------------------------------------------------------------------------------
; Interrupt Service Routine (ISR) definitions for each exceptions.
;--------------------------------------------------------------------------------------------------

[bits 64]

%macro save_context 0
    push rbp
    mov rbp, rsp

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
%endmacro

%macro load_context 0
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
%endmacro

global divide_error_routine                 ; Exception 0
global debug_routine                        ; Exception 1
global non_maskable_interrupt_routine       ; Exception 2
global break_point_routine                  ; Exception 3
global overflow_routine                     ; Exception 4
global bound_range_exceeded_routine         ; Exception 5
global invalid_opcode_routine               ; Exception 6
global device_not_available_routine         ; Exception 7
global double_fault_routine                 ; Exception 8
global coprocessor_segment_overrun_routine  ; Exception 9
global invalid_task_state_segment_routine   ; Exception 10
global segment_not_present_routine          ; Exception 11
global stack_full_routine                   ; Exception 12
global general_protection_routine           ; Exception 13
global page_fault_routine                   ; Exception 14
global x87_fpu_floating_point_error_routine ; Exception 15
global alignment_check_routine              ; Exception 16
global machine_check_routine                ; Exception 17
global simd_floating_point_routine          ; Exception 18
global virtualization_routine               ; Exception 19

global timeout_routine                      ; Interrupt 32
global keyboard_routine                     ; Interrupt 33

global null_routine

extern dummy_exception_handler
extern dummy_interrupt_handler

section .text

divide_error_routine:
    save_context

    mov rdi, 0
    call dummy_exception_handler

    load_context
    iret

debug_routine:
    save_context

    mov rdi, 1
    call dummy_exception_handler

    load_context
    iret

non_maskable_interrupt_routine:
    save_context

    mov rdi, 2
    call dummy_exception_handler

    load_context
    iret

break_point_routine:
    save_context

    mov rdi, 3
    call dummy_exception_handler

    load_context
    iret

overflow_routine:
    save_context

    mov rdi, 4
    call dummy_exception_handler

    load_context
    iret

bound_range_exceeded_routine:
    save_context

    mov rdi, 5
    call dummy_exception_handler

    load_context
    iret

invalid_opcode_routine:
    save_context

    mov rdi, 6
    call dummy_exception_handler

    load_context
    iret

device_not_available_routine:
    save_context

    mov rdi, 7
    call dummy_exception_handler

    load_context
    iret

double_fault_routine:
    save_context

    mov rdi, 8
    call dummy_exception_handler

    load_context
    iret

coprocessor_segment_overrun_routine:
    save_context

    mov rdi, 9
    call dummy_exception_handler

    load_context
    iret

invalid_task_state_segment_routine:
    save_context

    mov rdi, 10
    call dummy_exception_handler

    load_context
    iret

segment_not_present_routine:
    save_context

    mov rdi, 11
    call dummy_exception_handler

    load_context
    iret

stack_full_routine:
    save_context

    mov rdi, 12
    call dummy_exception_handler

    load_context
    iret

general_protection_routine:
    save_context

    mov rdi, 13
    call dummy_exception_handler

    load_context
    iret

page_fault_routine:
    save_context

    mov rdi, 14
    call dummy_exception_handler

    load_context
    iret

x87_fpu_floating_point_error_routine:
    save_context

    mov rdi, 15
    call dummy_exception_handler

    load_context
    iret

alignment_check_routine:
    save_context

    mov rdi, 16
    call dummy_exception_handler

    load_context
    iret

machine_check_routine:
    save_context

    mov rdi, 17
    call dummy_exception_handler

    load_context
    iret

simd_floating_point_routine:
    save_context

    mov rdi, 18
    call dummy_exception_handler

    load_context
    iret

virtualization_routine:
    save_context

    mov rdi, 19
    call dummy_exception_handler

    load_context
    iret

timeout_routine:
    save_context

    mov rdi, 32
    call dummy_interrupt_handler

    load_context
    iretq

keyboard_routine:
    save_context

    mov rdi, 33
    call dummy_interrupt_handler

    load_context
    iretq

null_routine:
    save_context

    mov rdi, 100
    call dummy_interrupt_handler

    load_context
    iretq
