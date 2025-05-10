#ifndef CHARACTER_CREATION_MODE_H
#define CHARACTER_CREATION_MODE_H

#include "../../game.h"
#include "../../game_data/character/character.h"
#include "../../io/input/input_handler.h"

/**
 * Initializes the character creation mode by allocating necessary memory
 * and setting up menus, spinners, and other required components. This
 * function prepares the environment for the player to create a character,
 * including resource and attribute selection menus.
 *
 * @return 0 on successful initialization, or a non-zero error code if
 * memory allocation or setup fails.
 */
int init_character_creation(void);

/**
 * Updates the character creation process based on the current input and
 * modifies the provided player data accordingly. This function handles
 * different stages of the character creation process such as pre-creation
 * initialization, name input, resource distribution, attribute distribution,
 * and waiting for confirmation after creation.
 *
 * @param input The current user input, which determines the flow and interaction
 *              in the character creation process.
 * @param player A pointer to the character structure being created or modified.
 *               This structure is updated throughout the character creation phases.
 * @return The current state of the game after handling the given input in the
 *         character creation process. Possible states include CHARACTER_CREATION
 *         or EXIT_GAME if the process should terminate.
 */
state_t update_character_creation(input_t input, character_t* player);

/**
 * Frees all allocated resources associated with the character creation mode.
 * This function iterates through the dynamically allocated strings used
 * within the character creation process, deallocates their memory, and then
 * frees the array that holds these strings. It ensures that no memory leaks
 * occur when transitioning out of the character creation mode.
 */
void shutdown_character_creation(void);

#endif//CHARACTER_CREATION_MODE_H
