#include "input_handler.h"

#include "../../../termbox2/termbox2.h"
#include "../../logger/logger.h"
#include "../../thread/thread_handler.h"

#define INPUT_BUFFER_SIZE 16

input_t input_buffer[INPUT_BUFFER_SIZE];
volatile int buffer_head = 0;
volatile int buffer_tail = 0;

char* text_buffer = NULL;
int text_buffer_length = 0;
int text_buffer_pos = 0;

bool input_handler_is_running = false;

/**
 * Starts the input handler thread.
 */
void start_input_handler_thread(void);

/**
 * The input handler thread function.
 * It will run in a loop and check for input events.
 * When an event is detected, it will store the input in the buffer.
 */
void input_handler_thread(void);

/**
 * Populates the text buffer with characters input from a terminal event.
 *
 * @param event The terminal event containing the input data, which includes key presses or character inputs.
 */
void populate_text_buffer(struct tb_event event);

void init_input_handler() {
    start_input_handler_thread();
}

input_t get_next_input() {
    if (buffer_head == buffer_tail) return NO_INPUT;// no input available

    const input_t input = input_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % INPUT_BUFFER_SIZE;
    return input;
}

char* start_text_input(const size_t max_length) {
    RETURN_WHEN_TRUE(text_buffer != NULL, NULL, "Input Handler", "Text input is already active");
    text_buffer = malloc(max_length);
    memset(text_buffer, '\0', max_length);
    text_buffer_length = (int) max_length;

    return text_buffer;
}

void end_text_input(void) {
    RETURN_WHEN_TRUE(text_buffer == NULL, , "Input Handler", "Text input is not active");
    free(text_buffer);
    text_buffer = NULL;
    text_buffer_length = 0;
    text_buffer_pos = 0;
}

void shutdown_input_handler(void) {
    input_handler_is_running = false;
}

void start_input_handler_thread(void) {
    input_handler_is_running = true;

    start_simple_thread(input_handler_thread);
}

void input_handler_thread() {
    bool running = true;
    while (running) {
        struct tb_event event;
        // wait for an event (blocking)
        const int ret = tb_poll_event(&event);

        input_t input = NO_INPUT;
        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP || event.ch == 'w')
                input = UP;
            else if (event.key == TB_KEY_ARROW_DOWN || event.ch == 's')
                input = DOWN;
            else if (event.key == TB_KEY_ARROW_LEFT || event.ch == 'a')
                input = LEFT;
            else if (event.key == TB_KEY_ARROW_RIGHT || event.ch == 'd')
                input = RIGHT;
            else if (event.ch == 'm')
                input = M;
            else if (event.ch == 'i')
                input = I;
            else if (event.ch == 'c')
                input = C;
            else if (event.ch == 'y' || event.ch == 'Y')
                input = Y;
            else if (event.key == TB_KEY_BACKSPACE || event.key == TB_KEY_BACKSPACE2)
                input = BACKSPACE;
            else if (event.key == TB_KEY_ENTER)
                input = ENTER;
            else if (event.key == TB_KEY_ESC)
                input = ESCAPE;
            else if (event.key == TB_KEY_CTRL_C)
                input = QUIT;

            if (input != NO_INPUT) {
                if ((buffer_tail - 1) % INPUT_BUFFER_SIZE == buffer_head) {
                    // buffer is full
                    log_msg(INFO, "Input Handler", "Input buffer is full");
                    buffer_tail = (buffer_tail + 1) % INPUT_BUFFER_SIZE;
                }
                input_buffer[buffer_head] = input;
                buffer_head = (buffer_head + 1) % INPUT_BUFFER_SIZE;
            }

            if (text_buffer != NULL) {
                // text input is active, populate the text buffer
                populate_text_buffer(event);
            }
        }

        if (!input_handler_is_running) {
            running = false;
        }
    }
}

void populate_text_buffer(const struct tb_event event) {
    if ((event.key == TB_KEY_BACKSPACE || event.key == TB_KEY_BACKSPACE2) && text_buffer_pos > 0) {
        text_buffer_pos--;
        text_buffer[text_buffer_pos] = '\0';
    } else if (event.ch != 0 && text_buffer_pos < text_buffer_length - 1) {
        text_buffer[text_buffer_pos++] = (char) event.ch;
        text_buffer[text_buffer_pos] = '\0'; // ensure the string is always null-terminated
    }
}
