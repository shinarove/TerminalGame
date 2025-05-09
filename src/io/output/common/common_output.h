#ifndef COMMON_OUTPUT_H
#define COMMON_OUTPUT_H

#include "../../colors.h"

typedef struct {
    char* title;
    char** options;
    int option_count;
    int selected_index;
    char* tailing_text;
} menu_t;

typedef struct {
    int active;// non-zero if the menu is active
} menu_arg_t;

typedef struct {
    int active;           // non-zero if the spinner is active
    char left_symbol;     // symbol to the left of the spinner
    char right_symbol;    // symbol to the right of the spinner
    int max_option_length;// maximum length of the options
} spinner_arg_t;

/**
 * Clears the terminal screen by invoking the termbox clear functionality.
 * @note Use this function with caution, preferably before a context switch occurs.
 * (Context switch is a switch between states, for example, MAP MODE -> MAIN MENU MODE)
 */
void clear_screen(void);

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


/**
 * Displays a simple menu at the specified position using the provided menu structure.
 * Highlights the currently selected menu option and displays additional trailing text if specified.
 * Ensures x and y values are valid by performing a boundary check.
 *
 * @param x The horizontal coordinate on the terminal screen where the menu will begin.
 * @param y The vertical coordinate on the terminal screen where the menu will begin.
 * @param menu A pointer to the menu_t structure containing the menu details such as title, options,
 *             selected option index, and trailing text. Must not be NULL.
 * @param args A pointer to a menu_arg_t structure that can hold extended information
 *                        related to the menu. Can be NULL if no additional information is required.
 */
void print_simple_menu(int x, int y, const menu_t* menu, menu_arg_t* args);

/**
 * Prints a spinner-style menu on the terminal screen at the specified coordinates.
 * The menu includes options displayed in a vertical list, along with a spinner control on the right side,
 * which highlights the selected option's spinner symbols based on the current selection index.
 * This function supports customizable spinner symbols and adjusts according to the maximum option length.
 *
 * @param x The x-coordinate for the top-left position of the menu.
 * @param y The y-coordinate for the top-left position of the menu.
 * @param menu Pointer to a simple_menu_t structure, which contains the menu options, title, selected index, and tailing text.
 *             Must not be NULL.
 * @param args Pointer to a spinner_additional_t structure, which specifies the spinner symbols and maximum option length.
 *                        Must not be NULL.
 */
void print_spinner_menu(int x, int y, const menu_t* menu, const spinner_arg_t* args);

#endif//COMMON_OUTPUT_H
