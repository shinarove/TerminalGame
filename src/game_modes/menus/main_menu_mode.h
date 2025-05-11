#ifndef MAIN_MENU_MODE_H
#define MAIN_MENU_MODE_H

#include "../../game.h"
#include "../../io/input/input_handler.h"

/**
 * Initializes the main menu by setting up its properties, allocating memory for menu strings,
 * and updating them with localized values. Observes local settings for updates.
 *
 * @return 0 on successful initialization, non-zero on failure (e.g., memory allocation failure).
 */
int init_main_menu(void);

/**
 * Updates the main menu state based on user input. It processes the input to navigate the menu,
 * execute actions such as starting the game, restarting, or changing game modes, and transitions
 * to the next state accordingly. If input is invalid, it logs a warning.
 *
 * @param input The user input that determines menu navigation or action, represented as an input_t value.
 * @return The next state of the game as a state_t value, based on the menu selection or input provided.
 */
state_t update_main_menu(input_t input);

/**
 * Frees all allocated memory associated with the main menu strings and resets
 * the pointer to the array of strings to NULL. This ensures proper resource
 * cleanup for the main menu components.
 *
 * Strings within the main menu array that were allocated are individually freed,
 * followed by freeing the array itself.
 *
 * No operation is performed if the main menu strings pointer is already NULL.
 */
void shutdown_main_menu(void);

#endif//MAIN_MENU_MODE_H
