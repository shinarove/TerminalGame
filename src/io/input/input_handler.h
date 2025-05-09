#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <stdlib.h>

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    M,
    I,
    C,
    ENTER,
    ESCAPE,
    QUIT,
    NO_INPUT
} input_t;

/**
 * Initializes the input handler and starts the input handler thread.
 */
void init_input_handler();

/**
 * Retrieves the next input from the input buffer. If the buffer is empty,
 * returns NO_INPUT to indicate no input is available.
 *
 * @return The next input of type input_t from the buffer, or NO_INPUT if the buffer is empty.
 */
input_t get_next_input(void);

/**
 * Starts a text input session by allocating a buffer for user input data
 * with a specified maximum length. The function ensures no additional
 * text input session is started if one is already active.
 *
 * @param max_length The maximum length of the text input buffer to allocate.
 * @return A pointer to the allocated input buffer, or NULL if a text
 *         input session is already active or upon allocation failure.
 */
char* start_text_input(size_t max_length);

/**
 * Ends the text input process and releases associated resources.
 *
 * Frees the memory allocated for the text buffer and resets its related state
 * variables. If text input is not currently active, the function logs an error
 * message and exits without performing any operations.
 */
void end_text_input(void);

/**
 * Shuts down the input handler and stops the input handler thread.
 */
void shutdown_input_handler(void);

#endif//INPUT_HANDLER_H
