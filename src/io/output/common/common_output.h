#ifndef COMMON_OUTPUT_H
#define COMMON_OUTPUT_H

#include "../../colors.h"
#include "../../menu.h"

/**
 * Clears the terminal screen by invoking the termbox clear functionality.
 * @note Use this function with caution, preferably before a context switch occurs.
 * (Context switch is a switch between states, for example, MAP MODE -> MAIN MENU MODE)
 */
void clear_screen(void);

/**
 * Clears a horizontal line on the terminal screen within specified bounds.
 * This is achieved by setting each character position within the range to a blank space
 * and applying default foreground and background colors.
 *
 * @param y The y-coordinate of the line to clear in the terminal space.
 * @param x_start The starting x-coordinate (inclusive) of the line to clear.
 * @param x_end The ending x-coordinate (exclusive) of the line to clear.
 */
void clear_line(int y, int x_start, int x_end);

/**
 * Prints a text string at the specified position on the terminal with the specified foreground and background colors.
 *
 * @param x The x-coordinate where the text will be placed.
 * @param y The y-coordinate where the text will be placed.
 * @param fg The foreground color of the text, specified as a value of type color_t.
 * @param bg The background color of the text, specified as a value of type color_t.
 * @param text A pointer to a null-terminated string containing the text to be displayed.
 *
 * The function logs an error and returns if the provided `text` is NULL. It uses the termbox library to output the
 * colored text by mapping the color_t enumeration values to corresponding termbox color attributes.
 */
void print_text(int x, int y, color_t fg, color_t bg, const char* text);

void print_text_f(int x, int y, color_t fg, color_t bg, const char* format, ...);

/**
 * Displays a simple menu at specified screen coordinates using termbox for rendering.
 * The menu includes a title, a list of selectable options, and trailing text.
 * Colors and selection behavior are controlled via the provided `menu` object.
 *
 * @param x The x-coordinate on the screen where the menu should start.
 * @param y The y-coordinate on the screen where the menu should start.
 * @param simple_menu A pointer to a `menu_t` structure containing the menu details,
 *             including its title, options, selected index, and rendering arguments.
 */
void print_simple_menu(int x, int y, const Menu* simple_menu);

/**
 * Renders a spinner menu on the terminal at the specified position.
 * The spinner menu includes a title, options, and symbols that represent a selected/active state.
 * Updates the appearance of the spinner menu based on the current selection and active state.
 *
 * @param x The x-coordinate of the starting position for the menu.
 * @param y The y-coordinate of the starting position for the menu.
 * @param spinner_menu A pointer to the spinner menu data structure containing menu details, symbols, and other configurations.
 * @note The function does not validate the terminal's rendering capability and assumes that the provided coordinates and spinner menu data are valid and properly initialized.
 */
void print_spinner_menu(int x, int y, const Menu* spinner_menu);

#endif//COMMON_OUTPUT_H
