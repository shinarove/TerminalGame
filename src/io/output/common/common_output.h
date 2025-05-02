#ifndef COMMON_OUTPUT_H
#define COMMON_OUTPUT_H

#include "../../colors.h"

typedef struct {
    char* title;
    char* options[];
    int option_count;
    int selected_index;
    char* tailing_text;
} menu_t;

/**
 * Clears the terminal screen by invoking the termbox clear functionality.
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
 * Displays a menu at the specified position on the terminal with a highlighted option to indicate selection.
 *
 * @param x The x-coordinate for the top-left corner of the menu.
 * @param y The y-coordinate for the top-left corner of the menu.
 * @param menu A pointer to a `menu_t` structure that contains the menu's title, options, selected index, and tailing text.
 *
 * If the provided `menu` is NULL, the function logs an error and returns without rendering the menu.
 * When the given `x` or `y` is negative, a warning is logged, and the position is clamped to (0, 0).
 *
 * Each menu option is displayed on a separate line, with the selected option highlighted using inverted colors.
 * The function outputs the menu's title above the options and any tailing text below the options.
 * After rendering, the terminal display is updated using `tb_present`.
 */
void print_menu(int x, int y, const menu_t* menu);

#endif//COMMON_OUTPUT_H
