#ifndef MENU_H
#define MENU_H

#include "input/input_handler.h"
#include "output/common/common_output.h"

/**
 * Handles navigation and selection within a basic menu using user input. The menu can be manipulated
 * by altering the selected index, or an action can be triggered based on specific input keys.
 *
 * @param input The user input that interacts with the menu (e.g., UP, DOWN, ENTER, ESCAPE, QUIT, etc.).
 * @param x The x-coordinate of the menu's display location.
 * @param y The y-coordinate of the menu's display location.
 * @param menu_to_handle A pointer to the menu structure containing menu details and the current state.
 * @return Returns:
 *         - The selected index of the menu if the ENTER key is pressed.
 *         - -1 if the ESCAPE key is pressed to indicate that the menu has been closed.
 *         - -2 if the QUIT key is pressed or if `menu_to_handle` is null to indicate an error or exit.
 *         - The total number of menu options if the input does not result in a selection or specific action.
 */
int handle_simple_menu(input_t input, int x, int y, menu_t* menu_to_handle);

/**
 * Handles the user input for navigating and interacting with a spinner menu. Depending on the input,
 * the current selection is updated, moved, or a specific action is triggered (e.g., selecting an option,
 * closing the menu, or quitting the game).
 *
 * @param input The user input that affects the menu (e.g., UP, DOWN, LEFT, RIGHT, ENTER, ESCAPE, etc.).
 * @param x The x-coordinate of where the menu should be displayed.
 * @param y The y-coordinate of where the menu should be displayed.
 * @param menu_to_handle A pointer to the menu structure that is being manipulated.
 * @param args A pointer to additional spinner-specific information (e.g., symbols and maximum option length).
 * @return Returns:
 *         - The selected index in the menu if the ENTER key was pressed.
 *         - -1 if the ESCAPE key was pressed to indicate the menu was closed.
 *         - -2 if the QUIT key was pressed or if a null pointer is encountered (e.g., `menu_to_handle` or `additional_info` is null).
 *         - The total number of adjusted options (option_count * 2) if the input does not lead to any specific action.
 */
int handle_spinner_menu(input_t input, int x, int y, menu_t* menu_to_handle,
                        const spinner_arg_t* args);

#endif//MENU_H
