#include "input_handler.h"

#include "../../logger/logger.h"
#include "../../../termbox2/termbox2.h"
#include "../../thread/thread_handler.h"

#define INPUT_BUFFER_SIZE 16

// === internal functions ===
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


// === global variables ===
input_t input_buffer[INPUT_BUFFER_SIZE];
int buffer_head = 0;
int buffer_tail = 0;

bool input_handler_is_running = false;

void start_input_handler_thread(void) {
    input_handler_is_running = true;

    start_simple_thread(input_handler_thread);
}

void input_handler_thread() {
    bool running = true;
    while (running) {
        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        input_t input = NO_INPUT;
        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP || event.ch == 'w') input = UP;
            else if (event.key == TB_KEY_ARROW_DOWN || event.ch == 's')
                input = DOWN;
            else if (event.key == TB_KEY_ARROW_LEFT || event.ch == 'a')
                input = LEFT;
            else if (event.key == TB_KEY_ARROW_RIGHT || event.ch == 'd')
                input = RIGHT;
            else if (event.key == 'm')
                input = MENU;
            else if (event.key == 'i')
                input = INVENTORY;
            else if (event.key == TB_KEY_ENTER)
                input = ENTER;
            else if (event.key == TB_KEY_ESC)
                input = ESCAPE;
            else if (event.key == TB_KEY_CTRL_Q)
                input = QUIT;

            if (input != NO_INPUT) {
                input_buffer[buffer_head] = input;
                buffer_head = (buffer_head + 1) % INPUT_BUFFER_SIZE;
                if ((buffer_tail - 1) % INPUT_BUFFER_SIZE == buffer_head) {
                    // buffer is full
                    log_msg(WARNING, "Input Handler", "Input buffer is full, next input will overwrite the oldest input");
                }
            }
        }

        if (!input_handler_is_running) {
            running = false;
        }
    }
}

void init_input_handler(void) {
    start_input_handler_thread();
}

input_t get_next_input(void) {
    if (buffer_head == buffer_tail) return NO_INPUT;// no input available

    const input_t input = input_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % INPUT_BUFFER_SIZE;
    return input;
}

void shutdown_input_handler(void) {
    // free resources if needed
}
