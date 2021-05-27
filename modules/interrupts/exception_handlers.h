#ifndef _INTERRUPTS_EXCEPTION_HANDLERS_H
#define _INTERRUPTS_EXCEPTION_HANDLERS_H

#include <stdint.h>

void dummy_exception_handler(uint8_t exception_number, uint64_t error_code);
void divide_error_handler(void);
void debug_handler(void);
void non_maskable_interrupt_handler(void);
void break_point_handler(void);
void overflow_handler(void);
void bound_range_exceeded_handler(void);
void invalid_opcode_handler(void);
void device_not_available_handler(void);
void double_fault_handler(void);
void coprocessor_segment_overrun_handler(void);
void invalid_task_state_segment_handler(void);
void segment_not_present_handler(void);
void stack_full_handler(void);
void general_protection_handler(void);
void page_fault_handler(void);
void x87_fpu_floating_point_error_handler(void);
void alignment_check_handler(void);
void machine_check_handler(void);
void simd_floating_point_handler(void);
void virtualization_handler(void);

#endif
