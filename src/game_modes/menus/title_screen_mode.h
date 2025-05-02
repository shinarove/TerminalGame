#ifndef TITLE_SCREEN_MODE_H
#define TITLE_SCREEN_MODE_H

#include "../../game.h"
#include "../../io/input/input_handler.h"

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

#endif //TITLE_SCREEN_MODE_H
