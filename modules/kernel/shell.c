// TODO: Separate buffers for contents, prompt, and etc. Composite them before render.

#include <stddef.h>

#include <drivers/keyboard/manager.h>
#include <general/circular_queue.h>
#include <general/memory_utils.h>
#include <kernel/console.h>
#include <memory/frame_allocator.h>

#include "shell.h"

#define SHELL_TAB_SIZE (4)

struct cursor {
    uint64_t row;
    uint64_t col;
};

struct shell_data {
    char *contents;
    size_t contents_buffer_size;
    size_t contents_row_size;
    size_t contents_col_size;
    struct cursor contents_cursor;

    struct cursor command_begin_cursor;
    struct cursor command_end_cursor;

    struct circular_queue_data exchange_queue_data;
    void *exchange_queue_buffer;
    size_t exchange_queue_size;
};

static struct shell_data global_shell_data;

static uint64_t get_index(uint64_t row, uint64_t col)
{
    return row * global_shell_data.contents_col_size + col;
}

static uint64_t get_next_index()
{
    return get_index(global_shell_data.contents_cursor.row, global_shell_data.contents_cursor.col);
}

static void shell_scroll_up(void)
{
    memory_copy(global_shell_data.contents,
            &global_shell_data.contents[global_shell_data.contents_col_size],
            (global_shell_data.contents_row_size - 1) * global_shell_data.contents_col_size);

    for (uint64_t i = 0; i < global_shell_data.contents_col_size; ++i) {
        global_shell_data.contents[get_index(global_shell_data.contents_row_size - 1, i)] = ' ';
    }

    if (global_shell_data.contents_cursor.row > 0) {
        --global_shell_data.contents_cursor.row;
        --global_shell_data.command_begin_cursor.row;
        --global_shell_data.command_end_cursor.row;
    }
}

static void newline(void)
{
    ++global_shell_data.contents_cursor.row;
    global_shell_data.contents_cursor.col = 0;

    if (global_shell_data.contents_cursor.row >= global_shell_data.contents_row_size) {
        shell_scroll_up();
    }
}

static void increase_cursor(void)
{
    ++global_shell_data.contents_cursor.col;

    if (global_shell_data.contents_cursor.col >= global_shell_data.contents_col_size) {
        newline();
    }
}

static void decrease_cursor(void)
{
    if (global_shell_data.contents_cursor.col == 0) {
        if (global_shell_data.contents_cursor.row == 0) {
            return;
        }

        --global_shell_data.contents_cursor.row;
        return;
    }

    --global_shell_data.contents_cursor.col;
}

static void insert_to_contents_buffer(char input)
{
    global_shell_data.contents[get_next_index()] = input;
    increase_cursor();
}

static inline void process_command(void)
{
    // This function will not work as expected if your command is too long to be stored in console.

    // TODO: This function just echo given command. Handle the it properly.

    struct cursor current_cursor = global_shell_data.command_begin_cursor;

    while (current_cursor.row != global_shell_data.command_end_cursor.row
            || current_cursor.col != global_shell_data.command_end_cursor.col) {
        insert_to_contents_buffer(
                global_shell_data.contents[get_index(current_cursor.row, current_cursor.col)]);

        ++current_cursor.col;

        if (current_cursor.col >= global_shell_data.contents_col_size) {
            ++current_cursor.row;
            current_cursor.col = 0;
        }

        if (current_cursor.row >= global_shell_data.contents_row_size) {
            // Something went wrong.
            return;
        }
    }

    insert_to_contents_buffer('!');
    newline();

    global_shell_data.command_begin_cursor = global_shell_data.contents_cursor;
    global_shell_data.command_end_cursor = global_shell_data.contents_cursor;
}

static inline bool exchange_queue_is_empty(void)
{
    return circular_queue_is_empty(&global_shell_data.exchange_queue_data);
}

static inline void process_outer_data(void)
{
    char data;

    while (circular_queue_pop(&global_shell_data.exchange_queue_data, &data) == 0) {
        insert_to_contents_buffer(data);
    }
}

int shell_insert(const byte_t *const data, size_t size)
{
    int result = 0;

    for (uint64_t i = 0; i < size; ++i) {
        result = circular_queue_push(&global_shell_data.exchange_queue_data, &data[i]);
        if (result != 0) {
            return -1;
        }
    }

    return 0;
}

static inline bool is_valid_input(char input)
{
    if (input == ' '
            || input == '\n'
            || input == '\t'
            || input == '`'
            || input == '~'
            || input == '!'
            || input == '@'
            || input == '#'
            || input == '$'
            || input == '%'
            || input == '^'
            || input == '&'
            || input == '*'
            || input == '('
            || input == ')'
            || input == '-'
            || input == '_'
            || input == '='
            || input == '+'
            || input == ASCII_BACKSPACE
            || input == '\\'
            || input == '|'
            || input == ';'
            || input == ':'
            || input == '\''
            || input == '"'
            || input == ','
            || input == '.'
            || input == '/'
            || input == '?'
            || ('a' <= input && input <= 'z')
            || ('A' <= input && input <= 'Z')
            || ('0' <= input && input <= '9')) {
        return true;
    }

    return false;
}

static inline void process_input(char input)
{
    if (input == '\n') {
        newline();
        process_command();
        return;
    }

    if (input == '\t') {
        for (uint64_t i = 0; i < SHELL_TAB_SIZE; ++i) {
            insert_to_contents_buffer(' ');
        }
        return;
    }

    if (input == ASCII_BACKSPACE) {
        if (global_shell_data.contents_cursor.row < global_shell_data.command_begin_cursor.row) {
            return;
        }

        if (global_shell_data.contents_cursor.row == global_shell_data.command_begin_cursor.row
                && global_shell_data.contents_cursor.col <= global_shell_data.command_begin_cursor.col) {
            return;
        }

        decrease_cursor();
        insert_to_contents_buffer(' ');
        decrease_cursor();
        return;
    }

    insert_to_contents_buffer(input);
    global_shell_data.command_end_cursor = global_shell_data.contents_cursor;
}

static inline int shell_initialize(void)
{
    const size_t required_buffer_size = console_get_width() * console_get_height();

    const size_t contents_frame_number
        = required_buffer_size % MEMORY_FRAME_SIZE == 0
        ? required_buffer_size / MEMORY_FRAME_SIZE
        : required_buffer_size / MEMORY_FRAME_SIZE + 1;
    const size_t exchange_queue_buffer_frame_number = contents_frame_number;

    // TODO: This implementation suffers from internal fragmentation. We need a better allocator.

    frame_t contents_frame = frame_allcoator_request(contents_frame_number);
    if (contents_frame == MEMORY_FRAME_NULL) {
        return -1;
    }

    frame_t exchange_queue_buffer_frame = frame_allcoator_request(exchange_queue_buffer_frame_number);
    if (exchange_queue_buffer_frame == MEMORY_FRAME_NULL) {
        frame_allocator_free(contents_frame, contents_frame_number);
        return -2;
    }

    global_shell_data.contents = contents_frame;
    global_shell_data.contents_buffer_size = contents_frame_number * MEMORY_FRAME_SIZE;
    global_shell_data.contents_row_size = console_get_height();
    global_shell_data.contents_col_size = console_get_width();
    global_shell_data.contents_cursor.row = 0;
    global_shell_data.contents_cursor.col = 0;

    global_shell_data.command_begin_cursor.row = 0;
    global_shell_data.command_begin_cursor.col = 0;
    global_shell_data.command_end_cursor.row = 0;
    global_shell_data.command_end_cursor.col = 0;

    global_shell_data.exchange_queue_buffer = exchange_queue_buffer_frame;
    global_shell_data.exchange_queue_size = exchange_queue_buffer_frame_number * MEMORY_FRAME_SIZE;
    circular_queue_initialize(&global_shell_data.exchange_queue_data, global_shell_data.exchange_queue_buffer,
            global_shell_data.exchange_queue_size, sizeof(char));

    for (uint64_t i = 0; i < global_shell_data.contents_buffer_size; ++i) {
        global_shell_data.contents[i] = ' ';
    }

    return 0;
}

int shell_start(void)
{
    char input;
    int result;

    result = keyboard_manager_initialize();
    if (result != 0) {
        return -1;
    }

    result = shell_initialize();
    if (result != 0) {
        return -2;
    }

    while (1) {
        if (!keyboard_manager_is_buffer_empty()) {
            result = keyboard_manager_get_input(&input);
            if (result == 0 && is_valid_input(input)) {
                process_input(input);
                console_draw(global_shell_data.contents,
                        global_shell_data.contents_row_size,
                        global_shell_data.contents_col_size);
            }
        }

        if (!exchange_queue_is_empty()) {
            process_outer_data();
            console_draw(global_shell_data.contents,
                    global_shell_data.contents_row_size,
                    global_shell_data.contents_col_size);
        }
    }
}
