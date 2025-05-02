#ifndef COMMON_OUTPUT_H
#define COMMON_OUTPUT_H

#include "../../colors.h"

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

#endif//COMMON_OUTPUT_H
