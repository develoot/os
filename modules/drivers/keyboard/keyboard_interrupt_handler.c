#include <cpu/port.h>
#include <general/circular_queue.h>
#include <interrupts/control_interrupts.h>
#include <interrupts/programmable_interrupt_controller.h>

#include "keyboard_interrupt_handler.h"

#define KEYBOARD_QUEUE_BUFFER (128)

static struct circular_queue_data keyboard_queue_data;
static scancode_t keyboard_queue_buffer[KEYBOARD_QUEUE_BUFFER];

void initialize_keyboard_queue(void)
{
    initialize_circular_queue(&keyboard_queue_data,
            keyboard_queue_buffer, sizeof(keyboard_queue_buffer), sizeof(scancode_t));
}

void keyboard_interrupt_handler(const uint8_t interrupt_number)
{
    disable_interrupts();

    const scancode_t scancode = read_port(keyboard0);

    circular_queue_push(&keyboard_queue_data, &scancode);

    notify_end_of_interrupt(interrupt_number - 32);

    enable_interrupts();
}

scancode_t get_scancode(void)
{
    scancode_t result;

    circular_queue_pop(&keyboard_queue_data, &result);

    return result;
}
