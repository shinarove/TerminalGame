#ifndef LEVEL_UP_MODE_H
#define LEVEL_UP_MODE_H

#include "../../game.h"
#include "../../game_data/character/character.h"
#include "../../io/input/input_handler.h"

/**
 * Initializes the Level-Up mode of the application.
 * This function allocates memory for storing level-up mode strings,
 * initializes the level-up menu configuration, sets up local observers
 * for updating locale-specific text, and prepares the system for handling
 * level-up related interactions. Logs an error message and returns a
 * non-zero value if memory allocation fails.
 *
 * @return 0 if initialization is successful, or 1 if memory allocation fails.
 */
int init_lvl_up_mode(void);

/**
 * Updates the Level-Up mode of the game based on user input.
 * This function manages the display of Level-Up mode components,
 * including the current title and player status, and processes
 * input for navigating and performing actions in the Level-Up mode menu.
 * Handles different states such as attribute selection and completion
 * after leveling up. Ensures input is interpreted appropriately and
 * the game state transitions as required.
 *
 * @param input The input received from the user (e.g., key presses).
 * @param player Pointer to the player character, whose attributes
 *               may be updated during the Level-Up process.
 * @return The updated game state, typically remaining in LVL_UP_MODE
 *         unless a transition to another state is triggered.
 */
state_t update_lvl_up_mode(input_t input, character_t* player);

/**
 * Shuts down the Level-Up mode of the application.
 * This function deallocates all memory associated with the level-up mode string
 * storage. It iterates through the stored strings, freeing each one, and then
 * releases the primary array holding the string pointers. Ensures that no memory
 * leaks occur related to level-up mode strings.
 */
void shutdown_lvl_up_mode(void);

#endif//LEVEL_UP_MODE_H
