#include <cpu/port.h>
#include <general/circular_queue.h>
#include <interrupts/control_register.h>
#include <interrupts/controller.h>
#include <interrupts/exception_vector_size.h>

#include "interrupt_handler.h"
#include "port.h"

#define GLOBAL_KEYBOARD_QUEUE_BUFFER_SIZE (64)

static struct circular_queue_data global_keyboard_queue_data;
static scancode_t global_keyboard_queue_buffer[GLOBAL_KEYBOARD_QUEUE_BUFFER_SIZE];

void keyboard_interrupt_handler_initialize(void)
{
    circular_queue_initialize(&global_keyboard_queue_data,
            global_keyboard_queue_buffer, sizeof(global_keyboard_queue_buffer), sizeof(scancode_t));
}

void keyboard_interrupt_handler(const uint8_t interrupt_number)
{
    if (is_output_buffer_full() == false) {
        return;
    }

    /*
     * TODO: Note that internal state of the queue might be invalid because of additional interrupt
     * occured while processing code below.
     */
    const scancode_t scancode = port_read(keyboard0);
    circular_queue_push(&global_keyboard_queue_data, &scancode);
    interrupt_controller_notify_end(interrupt_number - EXCEPTION_VECTOR_SIZE);
}

bool keyboard_interrupt_handler_queue_is_empty(void)
{
    return global_keyboard_queue_data.entry_number == 0;
}

scancode_t keyboard_interrupt_handler_get_scancode(void)
{
    scancode_t scancode;
    circular_queue_pop(&global_keyboard_queue_data, &scancode);
    return scancode;
}
