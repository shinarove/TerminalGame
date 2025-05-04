#ifndef TITLE_SCREEN_MODE_H
#define TITLE_SCREEN_MODE_H

#include "../../game.h"
#include "../../io/input/input_handler.h"

/**
 * Initializes the title screen by setting up its menu structure and loading necessary resources.
 * This function allocates memory for the title screen strings, configures the menu options,
 * and registers a handler to update local text dynamically. It ensures the title screen is
 * correctly prepared for display and interaction.
 *
 * @return An integer status value:
 *         - 0: Initialization succeeded.
 *         - 1: Memory allocation for title screen strings failed.
 */
int init_title_screen(void);

/**
 * Updates the title screen state based on user input and handles menu interactions.
 * This function modifies the title screen menu, processes input to navigate or select
 * menu options, and determines the next state for the game.
 *
 * @param input The user input to the process, represented by an input_t value.
 *              It corresponds to keypress events that navigate the menu or make selections.
 * @return The next state of the game, represented by a state_t value.
 *         - TITLE_SCREEN: Remains in the title screen.
 *         - MAP_GENERATION: Transitions to the map generation process.
 *         - EXIT_GAME: Exits the game.
 */
state_t update_title_screen(input_t input);

/**
 * Cleans up and releases all resources associated with the title screen.
 * This function ensures any dynamically allocated memory for the title screen
 * strings is properly freed and resets the related pointers to prevent
 * dangling references. It handles deallocation for all elements of the
 * `title_screen_strings` array and the array itself, guaranteeing a complete
 * teardown of the title screen.
 *
 * This function is designed to prevent memory leaks during application shutdown
 * or when the title screen is no longer needed.
 */
void shutdown_title_screen(void);

#endif//TITLE_SCREEN_MODE_H
