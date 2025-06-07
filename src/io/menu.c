#include "menu.h"

#include "output/common/common_output.h"
#include "../logger/logger.h"

int handle_simple_menu(Menu* self, input_t input, int x, int y);
int handle_spinner_menu(Menu* self, input_t input, int x, int y);

static const Menu_VTable simple_menu_vtable = {
    .handle_menu = handle_simple_menu
};

static const Menu_VTable spinner_menu_vtable = {
    .handle_menu = handle_spinner_menu
};

Menu* init_simple_menu(char* title, char** options, const int option_count, char* tailing_text) {
    Menu* menu = malloc(sizeof(Menu));
    RETURN_WHEN_NULL(menu, NULL, "Menu", "Failed to allocate memory for simple menu");

    menu->title = title;
    menu->options = options;
    menu->selected_index = 0;
    menu->option_count = option_count;
    menu->tailing_text = tailing_text;

    menu->args.mode = ACTIVE;
    menu->args.selected_fg = BLACK;
    menu->args.selected_bg = WHITE;
    menu->args.unselected_fg = WHITE;
    menu->args.unselected_bg = DEFAULT;
    menu->args.max_option_length = 64;

    menu->vtable = &simple_menu_vtable;
    return menu;
}

Menu* init_spinner_menu(char* title, char** options, const int option_count, char* tailing_text) {
    Menu* menu = init_simple_menu(title, options, option_count, tailing_text);
    if (menu != NULL) {
        menu->vtable = &spinner_menu_vtable;
    }
    return menu;
}

void destroy_menu(Menu* menu) {
    if (menu != NULL) {
        free(menu);
    }
}

int handle_simple_menu(Menu* self, const input_t input, const int x, const int y) {
    RETURN_WHEN_NULL(self, -2, "Menu", "In `handle_simple_menu` given menu is NULL");

    print_simple_menu(x, y, self);

    if (self->args.mode != ACTIVE) {
        // menu is not active, do nothing
        return self->option_count;
    }
    if (self->option_count == 0) {
        // menu has no options, do nothing
        return self->option_count;
    }

    switch (input) {
        case UP:
            self->selected_index = (self->selected_index - 1 + self->option_count) % self->option_count;
            break;
        case DOWN:
            self->selected_index = (self->selected_index + 1) % self->option_count;
            break;
        case ENTER:
            return self->selected_index;
        case ESCAPE:
            return -1;// return -1 to indicate that the menu was closed
        case QUIT:
            return -2;// return -2 to indicate that the game should exit
        default:
            break;// do nothing
    }
    return self->option_count;// return the number of options to indicate that the input was not handled
}

int handle_spinner_menu(Menu* self, const input_t input, const int x, const int y) {
    RETURN_WHEN_NULL(self, -2, "Menu", "Menu to handle is NULL");

    print_spinner_menu(x, y, self);

    const int real_option_count = self->option_count * 2;

    switch (input) {
        case UP:
            self->selected_index = (self->selected_index - 2 + real_option_count) % real_option_count;
            break;
        case DOWN:
            self->selected_index = (self->selected_index + 2) % real_option_count;
            break;
        case LEFT:
            // move the marker to the left if the marker was on the right side
            if (self->selected_index % 2 != 0) self->selected_index--;
            break;
        case RIGHT:
            // move the marker to the right if the marker was on the left side
            if (self->selected_index % 2 == 0) self->selected_index++;
            break;
        case ENTER:
            return self->selected_index;
        case ESCAPE:
            return -1;// return -1 to indicate that the menu was closed
        case QUIT:
            return -2;// return -2 to indicate that the game should exit
        default:
            break;// do nothing
    }
    // return the number of options * 2 to indicate that the input was not handled
    return real_option_count;
}
