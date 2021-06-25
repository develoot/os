#include <stddef.h>

#include <drivers/keyboard/manager.h>
#include <general/memory.h>
#include <general/string.h>
#include <kernel/console.h>
#include <memory/frame_allocator.h>

#include "shell.h"

#define PROMPT         ("$> ")
#define PROMPT_SIZE    (string_length(PROMPT))
#define SHELL_TAB_SIZE (4)

struct cursor {
    uint64_t row;
    uint64_t col;
};

struct buffer_data {
    char *items;
    size_t size;
    size_t row_size;
    size_t col_size;
    struct cursor cursor;
};

// TODO: mutex
struct shell_data {
    struct buffer_data render;
    struct buffer_data contents;
    struct buffer_data command;
    struct buffer_data exchange;
};

static struct shell_data global_shell_data;

static uint64_t buffer_get_index(const struct buffer_data *const buffer, uint64_t row, uint64_t col)
{
    return row * buffer->col_size + col;
}

static uint64_t buffer_get_next_index(const struct buffer_data *const buffer)
{
    return buffer_get_index(buffer, buffer->cursor.row, buffer->cursor.col);
}

static void buffer_scroll_up(struct buffer_data *const buffer)
{
    memory_copy(buffer->items,
            &buffer->items[buffer->col_size], (buffer->row_size - 1) * buffer->col_size);

    for (uint64_t i = 0; i < buffer->col_size; ++i) {
        buffer->items[buffer_get_index(buffer, buffer->row_size - 1, i)] = ' ';
    }

    if (buffer->cursor.row > 0) {
        --buffer->cursor.row;
    }
}

static void buffer_newline(struct buffer_data *const buffer)
{
    ++buffer->cursor.row;
    buffer->cursor.col = 0;

    if (buffer->cursor.row >= buffer->row_size) {
        buffer_scroll_up(buffer);
    }
}

static void buffer_initialize(struct buffer_data *const buffer,
        char *items, size_t size, size_t row_size, size_t col_size)
{
    buffer->items = items;
    buffer->size = size;
    buffer->row_size = row_size;
    buffer->col_size = col_size;
    buffer->cursor.row = 0;
    buffer->cursor.col = 0;
}

static void buffer_increase_cursor(struct buffer_data *const buffer)
{
    ++buffer->cursor.col;

    if (buffer->cursor.col >= buffer->col_size) {
        buffer_newline(buffer);
    }
}

static int buffer_decrease_cursor(struct buffer_data *const buffer)
{
    if (buffer->cursor.col == 0) {
        if (buffer->cursor.row == 0) {
            return -1;
        }

        --buffer->cursor.row;
        return 0;
    }

    --buffer->cursor.col;
    return 0;
}

static void buffer_push(struct buffer_data *const buffer, char input)
{
    buffer->items[buffer_get_next_index(buffer)] = input;
    buffer_increase_cursor(buffer);
}

static void buffer_push_string(struct buffer_data *const buffer, const char *const string)
{
    for (uint64_t i = 0; string[i] != '\0'; ++i) {
        buffer_push(buffer, string[i]);
    }
}

static int buffer_pop(struct buffer_data *const buffer)
{
    if (buffer_decrease_cursor(buffer) != 0) {
        return -1;
    }

    buffer->items[buffer_get_next_index(buffer)] = ' ';
    return 0;
}

static inline void process_command(void)
{
    for (uint64_t i = 0; i < buffer_get_index(&global_shell_data.command,
                global_shell_data.command.cursor.row, global_shell_data.command.cursor.col); ++i) {
        buffer_push(&global_shell_data.contents, global_shell_data.command.items[i]);
    }
    buffer_newline(&global_shell_data.contents);

    buffer_push_string(&global_shell_data.contents, "Unknown command: ");

    for (uint64_t i = PROMPT_SIZE; i < buffer_get_index(&global_shell_data.command,
                global_shell_data.command.cursor.row, global_shell_data.command.cursor.col); ++i) {
        buffer_push(&global_shell_data.contents, global_shell_data.command.items[i]);
    }
    buffer_newline(&global_shell_data.contents);

    global_shell_data.command.cursor.row = 0;
    global_shell_data.command.cursor.col = PROMPT_SIZE;
}

static inline bool is_valid_input(char input)
{
    if (input == ' ' || input == '\n' || input == '\t'
            || input == '`'             || input == '~'             || input == '!'
            || input == '@'             || input == '#'             || input == '$'
            || input == '%'             || input == '^'             || input == '&'
            || input == '*'             || input == '('             || input == ')'
            || input == '-'             || input == '_'             || input == '='
            || input == '+'             || input == ASCII_BACKSPACE || input == '['
            || input == '{'             || input == ']'             || input == '}'
            || input == '\\'            || input == '|'             || input == ';'
            || input == ':'             || input == '\''            || input == '"'
            || input == ','             || input == '<'             || input == '.'
            || input == '>'             || input == '/'             || input == '?'
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
        process_command();
        return;
    }

    if (input == '\t') {
        for (uint64_t i = 0; i < SHELL_TAB_SIZE; ++i) {
            buffer_push(&global_shell_data.command, ' ');
        }
        return;
    }

    if (input == ASCII_BACKSPACE) {
        buffer_pop(&global_shell_data.command);
        return;
    }

    buffer_push(&global_shell_data.command, input);
}

static inline void buffer_push_prompt(struct buffer_data *const buffer)
{
    // Allocating buffer for prompt would be more flexible but complicated.
    buffer_push_string(buffer, PROMPT);
}

static bool is_exchange_buffer_empty(void)
{
    if (global_shell_data.exchange.cursor.row > 0 || global_shell_data.exchange.cursor.col > 0) {
        return false;
    }

    return true;
}

static inline void process_exchange_buffer(void)
{
    for (uint64_t i = 0; i < buffer_get_index(&global_shell_data.exchange,
                global_shell_data.exchange.cursor.row, global_shell_data.exchange.cursor.col); ++i) {
        buffer_push(&global_shell_data.contents, global_shell_data.exchange.items[i]);
    }

    global_shell_data.exchange.cursor.row = 0;
    global_shell_data.exchange.cursor.col = 0;
}

static inline int shell_initialize(void)
{
    const uint64_t console_width = console_get_width();
    const uint64_t console_height = console_get_height();
    const size_t required_buffer_size = console_width * console_height;

    const size_t render_frame_number = required_buffer_size % MEMORY_FRAME_SIZE == 0
        ? required_buffer_size / MEMORY_FRAME_SIZE
        : required_buffer_size / MEMORY_FRAME_SIZE + 1;
    const size_t contents_frame_number = render_frame_number;
    const size_t command_frame_number = render_frame_number;
    const size_t exchange_frame_number = render_frame_number;

    // TODO: This implementation suffers from internal fragmentation. We need a better allocator.

    frame_t render_frame = frame_allcoator_request(render_frame_number);
    if (render_frame == MEMORY_FRAME_NULL) {
        return -1;
    }

    frame_t contents_frame = frame_allcoator_request(contents_frame_number);
    if (contents_frame == MEMORY_FRAME_NULL) {
        frame_allocator_free(render_frame, render_frame_number);
        return -2;
    }

    frame_t command_frame = frame_allcoator_request(command_frame_number);
    if (command_frame == MEMORY_FRAME_NULL) {
        frame_allocator_free(render_frame, render_frame_number);
        frame_allocator_free(contents_frame, contents_frame_number);
        return -3;
    }

    frame_t exchange_frame = frame_allcoator_request(exchange_frame_number);
    if (exchange_frame == MEMORY_FRAME_NULL) {
        frame_allocator_free(render_frame, render_frame_number);
        frame_allocator_free(contents_frame, contents_frame_number);
        frame_allocator_free(command_frame, command_frame_number);
        return -4;
    }

    buffer_initialize(&global_shell_data.render, render_frame,
            render_frame_number * MEMORY_FRAME_SIZE, console_height, console_width);

    buffer_initialize(&global_shell_data.contents, contents_frame,
            contents_frame_number * MEMORY_FRAME_SIZE, console_height, console_width);

    buffer_initialize(&global_shell_data.command, command_frame,
            command_frame_number * MEMORY_FRAME_SIZE, console_height, console_width);
    buffer_push_prompt(&global_shell_data.command);

    buffer_initialize(&global_shell_data.exchange, exchange_frame,
            exchange_frame_number * MEMORY_FRAME_SIZE, console_height, console_width);

    for (uint64_t i = 0; i < global_shell_data.render.size; ++i) {
        global_shell_data.render.items[i] = ' ';
    }

    for (uint64_t i = 0; i < global_shell_data.contents.size; ++i) {
        global_shell_data.contents.items[i] = ' ';
    }

    return 0;
}

static void composite(void)
{
    for (uint64_t i = 0; i < global_shell_data.render.size; ++i) {
        global_shell_data.render.items[i] = ' ';
    }

    global_shell_data.render.cursor.row = 0;
    global_shell_data.render.cursor.col = 0;

    for (uint64_t i = 0; i < buffer_get_index(&global_shell_data.contents,
                global_shell_data.contents.cursor.row, global_shell_data.contents.cursor.col); ++i) {
        buffer_push(&global_shell_data.render, global_shell_data.contents.items[i]);
    }

    for (uint64_t i = 0; i < buffer_get_index(&global_shell_data.command,
                global_shell_data.command.cursor.row, global_shell_data.command.cursor.col); ++i) {
        buffer_push(&global_shell_data.render, global_shell_data.command.items[i]);
    }
}

int shell_start(void)
{
    char input;
    int result;

    result = keyboard_initialize();
    if (result != 0) {
        return -1;
    }

    result = shell_initialize();
    if (result != 0) {
        return -2;
    }

    while (1) {
        if (!keyboard_is_buffer_empty()) {
            result = keyboard_get_input(&input);
            if (result == 0 && is_valid_input(input)) {
                process_input(input);
                composite();
                console_draw(global_shell_data.render.items,
                        global_shell_data.render.row_size,
                        global_shell_data.render.col_size);
            }
        }

        if (!is_exchange_buffer_empty()) {
            process_exchange_buffer();
            composite();
            console_draw(global_shell_data.render.items,
                    global_shell_data.render.row_size,
                    global_shell_data.render.col_size);
        }
    }
}

int shell_insert(const byte_t *const data, size_t size)
{
    for (uint64_t i = 0; i < size; ++i) {
        if (!is_valid_input(data[i])) {
            continue;
        }

        if (data[i] == '\n') {
            buffer_newline(&global_shell_data.exchange);
            continue;
        }

        if (data[i] == '\t') {
            for (uint64_t i = 0; i < SHELL_TAB_SIZE; ++i) {
                buffer_push(&global_shell_data.exchange, ' ');
            }
            continue;
        }

        if (data[i] == ASCII_BACKSPACE) {
            buffer_pop(&global_shell_data.exchange);
            continue;
        }

        buffer_push(&global_shell_data.exchange, data[i]);
    }

    return 0;
}
