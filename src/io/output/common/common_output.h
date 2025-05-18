#ifndef COMMON_OUTPUT_H
#define COMMON_OUTPUT_H

#include "../../colors.h"

typedef struct {
    int active;// non-zero if the menu is active
    color_t selected_fg;
    color_t selected_bg;
    color_t unselected_fg;
    color_t unselected_bg;
} menu_arg_t;

typedef struct {
    char* title;
    char** options;
    int option_count;
    int selected_index;
    char* tailing_text;
    menu_arg_t* args;
} menu_t;

typedef struct {
    menu_t* menu;
    char left_symbol;     // symbol to the left of the spinner
    char right_symbol;    // symbol to the right of the spinner
    int max_option_length;// maximum length of the options
} spinner_menu_t;

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
 * @param menu A pointer to a `menu_t` structure containing the menu details,
 *             including its title, options, selected index, and rendering arguments.
 */
void print_simple_menu(int x, int y, const menu_t* menu);

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
void print_spinner_menu(int x, int y, const spinner_menu_t* spinner_menu);

#endif//COMMON_OUTPUT_H
