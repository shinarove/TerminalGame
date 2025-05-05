#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

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
 * Shuts down the input handler and stops the input handler thread.
 */
void shutdown_input_handler(void);

#endif//INPUT_HANDLER_H
