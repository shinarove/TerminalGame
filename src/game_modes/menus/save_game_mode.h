#ifndef SAVE_GAME_MODE_H
#define SAVE_GAME_MODE_H

#include "../../game.h"
#include "../../game_data/save_file_handler.h"
#include "../../io/input/input_handler.h"

/**
 * Initializes the save game mode by setting up required data structures and
 * loading localized strings for the save game menu. This function allocates
 * memory for saving game mode strings, initializes the save game menu, sets
 * default values, and prepares the menu options for user interaction.
 * Additionally, it observes local changes to ensure strings are updated
 * dynamically.
 *
 * @return 0 on success, or 1 if memory allocation for save game mode strings fails.
 */
int init_save_game_mode(void);

/**
 * Prepares the save game mode by initializing the save game menu,
 * setting slot-specific strings, and verifying the provided game state.
 * This function configures the structure and presentation of save slots,
 * including displaying the save slot status and information (e.g., save dates),
 * and ensures consistency in game state data for saving.
 *
 * The function also validates the number of save slots and sets the game state
 * for future save operations. If the provided save info has unexpected lengths
 * or the game state is invalid, the function logs errors and updates the game
 * state to indicate an exit scenario.
 *
 * @param game_state Pointer to the game state structure containing the current
 *                   game data that should be saved.
 *
 * @return The next game state, which is typically SAVE_GAME on success, or
 *         EXIT_GAME if an error occurs (e.g., invalid save info length,
 *         null game state).
 */
state_t prepare_save_game_mode(game_state_t* game_state);

/**
 * Updates the save game mode based on the current state and user input. This function
 * processes the `sg_state` to navigate through different stages of saving the game,
 * handling menu interactions, confirmation prompts, success or failure feedback, and
 * input events. It ensures that the save game process proceeds as expected, according
 * to the system state and user actions.
 *
 * @param input The user input received from the input handler. It determines the
 *              navigation and interaction within the save game mode, such as selecting
 *              menu options, confirming overwriting, or exiting the save process.
 * @return The current state of the game after processing the save game mode logic.
 */
state_t update_save_game_mode(input_t input);

/**
 * Releases all resources allocated for the save game mode and performs
 * the necessary cleanup. This function iterates through and frees all save game
 * strings if they were dynamically allocated, as well as the container
 * holding these strings. It ensures memory is properly deallocated to
 * prevent memory leaks. Calling this function is essential before shutting
 * down the application or reinitializing the save game mode.
 */
void shutdown_save_game_mode(void);

#endif//SAVE_GAME_MODE_H
