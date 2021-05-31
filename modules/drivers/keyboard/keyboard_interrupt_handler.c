#include <cpu/port.h>
#include <general/circular_queue.h>
#include <interrupts/control_interrupts.h>
#include <interrupts/exception_vector_size.h>
#include <interrupts/programmable_interrupt_controller.h>

#include "control_keyboad_port.h"

#include "keyboard_interrupt_handler.h"

#define GLOBAL_KEYBOARD_QUEUE_BUFFER_SIZE (64)

static struct circular_queue_data global_keyboard_queue_data;
static scancode_t global_keyboard_queue_buffer[GLOBAL_KEYBOARD_QUEUE_BUFFER_SIZE];

void initialize_keyboard_queue(void)
{
    initialize_circular_queue(&global_keyboard_queue_data,
            global_keyboard_queue_buffer, sizeof(global_keyboard_queue_buffer), sizeof(scancode_t));
}

void keyboard_interrupt_handler(const uint8_t interrupt_number)
{
    if (is_output_buffer_full() == false) {
        return;
    }

    const scancode_t scancode = port_read(keyboard0);
    circular_queue_push(&global_keyboard_queue_data, &scancode);
    notify_end_of_interrupt(interrupt_number - EXCEPTION_VECTOR_SIZE);
}

bool keyboard_queue_is_empty(void)
{
    return global_keyboard_queue_data.entry_number == 0;
}

scancode_t get_scancode_from_queue(void)
{
    scancode_t scancode;
    circular_queue_pop(&global_keyboard_queue_data, &scancode);
    return scancode;
}
