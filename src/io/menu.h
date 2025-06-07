#ifndef MENU_H
#define MENU_H

#include "input/input_handler.h"
#include "output/common/common_output.h"

typedef struct Menu Menu;
typedef struct Menu_VTable Menu_VTable;

typedef enum {
    ACTIVE,
    INACTIVE_WITH_SEL,
    INACTIVE_WOUT_SEL,
} menu_mode_t;

typedef struct {
    menu_mode_t mode;
    color_t selected_fg;
    color_t selected_bg;
    color_t unselected_fg;
    color_t unselected_bg;
    int max_option_length;
} menu_arg_t;

struct Menu {
    char* title;
    char** options;
    int option_count;
    int selected_index;
    char* tailing_text;
    menu_arg_t args;
    const Menu_VTable* vtable;
};

struct Menu_VTable {
    /**
     * Handles the menu interaction based on user input.
     *
     * @param self Pointer to the Menu structure.
     * @param input The user input that interacts with the menu (e.g., UP, DOWN, ENTER, ESCAPE, QUIT, etc.).
     * @param x The top-left x-coordinate of the menu's display location.
     * @param y The top-left y-coordinate of the menu's display location.
     * @return Returns:
     *        - (-2): the game should exit (QUIT key pressed or menu is NULL).
     *        - (-1): the menu was closed (ESCAPE key pressed).
     *        - (selected index): the selected index of the menu if the ENTER key is pressed.
     *        - (option_count): the total number of options if the input does not result in a selection or specific action.
     */
    int (*handle_menu)(Menu* self, input_t input, int x, int y);
};

Menu* init_simple_menu(char* title, char** options, int option_count, char* tailing_text);

Menu* init_spinner_menu(char* title, char** options, int option_count, char* tailing_text);

void destroy_menu(Menu* menu);

#endif//MENU_H
