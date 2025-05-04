#ifndef CHANGE_LANGUAGE_MODE_H
#define CHANGE_LANGUAGE_MODE_H

#include "../../game.h"
#include "../../io/input/input_handler.h"

/**
 * Initializes the "Change Language" feature by allocating memory for language-specific
 * strings, setting up the menu structure, and observing language updates.
 *
 * This function allocates memory for storing localized language strings, configures
 * the change language menu, and links the update function to observe language changes.
 * If the allocation fails, an error message is logged and a non-zero value is returned.
 *
 * @return 0 on successful initialization, non-zero value if memory allocation fails.
 */
int init_change_language(void);

/**
 * Handles the "Change Language" state, allowing the user to change the game's language
 * or return to the previous state based on the provided input.
 *
 * This function displays the "Change Language" menu, processes user input, and updates
 * the game state accordingly. If a valid language option is selected, the language is
 * updated, and the state transitions back to the calling state. Invalid input or errors
 * in changing the language result in a warning or transition to the appropriate state.
 *
 * @param input The user input to process for handling navigation or language selection.
 * @param called_from The state from which the "Change Language" feature was called,
 *                    used to return after successful language selection or user exit.
 * @return The next state to transition to, which may be the calling state, EXIT_GAME
 *         if an error occurred, or CHANGE_LANGUAGE to remain in this state.
 */
state_t update_change_language(input_t input, state_t called_from);

/**
 * Shuts down the "Change Language" feature by deallocating any allocated memory
 * for language-specific strings and resetting associated resources.
 *
 * This function frees memory allocated for the array of language strings and its
 * individual elements. It ensures proper cleanup to prevent memory leaks.
 * If the language strings array is already NULL, the function exits without further action.
 */
void shutdown_change_language(void);

#endif //CHANGE_LANGUAGE_MODE_H
