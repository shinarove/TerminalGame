#ifndef MENU_H
#define MENU_H

#include "input/input_handler.h"
#include "output/common/common_output.h"

/**
 * Handles the user input for navigating and interacting with a menu. Depending on the input,
 * the menu selection is updated, or a specific action is triggered (e.g., closing the menu or quitting the game).
 *
 * @param input The user input that affects the menu (e.g., UP, DOWN, ENTER, ESCAPE, etc.).
 * @param x The x-coordinate of where the menu should be displayed.
 * @param y The y-coordinate of where the menu should be displayed.
 * @param menu_to_handle A pointer to the menu structure to be handled and updated.
 * @return Returns:
 *         - The selected menu index if the ENTER key was pressed.
 *         - -1 if the ESCAPE key was pressed to indicate the menu was closed.
 *         - -2 if the QUIT key was pressed to indicate quitting the game.
 *         - The total number of options in the menu if the input did not lead to any specific action.
 */
int handle_menu(input_t input, int x, int y, menu_t* menu_to_handle);

#endif//MENU_H
