#ifndef MENU_H
#define MENU_H

#include "input/input_handler.h"
#include "output/common/common_output.h"

/**
 * Handles the user's menu input and updates the provided menu accordingly.
 * Depending on the user's input, the function can update the selected menu option,
 * close the menu, or return the currently selected option.
 *
 * @param input The user's input, represented by an input_t value,
 *              such as UP, DOWN, ENTER, or ESCAPE.
 * @param x The x-coordinate where the menu will be displayed.
 * @param y The y-coordinate where the menu will be displayed.
 * @param menu_to_handle A pointer to the menu_t structure representing the menu to be handled.
 *                       This must be a non-NULL pointer.
 * @return An integer indicating the outcome:
 *         - The index of the selected option if the ENTER input is received.
 *         - -1 if the ESCAPE input is received, indicating the menu was closed.
 *         - The number of options in the menu when the input does not directly select or close the menu.
 *         - -1 if the provided menu_to_handle pointer is NULL.
 */
int handle_menu(input_t input, int x, int y, menu_t* menu_to_handle);

#endif//MENU_H
