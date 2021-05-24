#ifndef _ASM_INTERRUPTS_INTERRUPT_SERVICE_ROUTINES_H
#define _ASM_INTERRUPTS_INTERRUPT_SERVICE_ROUTINES_H

// Exceptions routines.
void divide_error_routine(void);
void debug_routine(void);
void non_maskable_interrupt_routine(void);
void break_point_routine(void);
void overflow_routine(void);
void bound_range_exceeded_routine(void);
void invalid_opcode_routine(void);
void device_not_available_routine(void);
void double_fault_routine(void);
void coprocessor_segment_overrun_routine(void);
void invalid_task_state_segment_routine(void);
void segment_not_present_routine(void);
void stack_full_routine(void);
void general_protection_routine(void);
void page_fault_routine(void);
void x87_fpu_floating_point_error_routine(void);
void alignment_check_routine(void);
void machine_check_routine(void);
void simd_floating_point_routine(void);
void virtualization_routine(void);

// Interrupt routines.
void timeout_routine(void);
void keyboard_routine(void);

// A place holder.
void null_routine(void);

#endif
