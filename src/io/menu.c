#include "menu.h"

#include "../logger/logger.h"

int handle_simple_menu(const input_t input, const int x, const int y, simple_menu_t* menu_to_handle) {
    RETURN_WHEN_NULL(menu_to_handle, -2, "Menu", "Menu to handle is NULL");

    print_menu_s(x, y, menu_to_handle);

    switch (input) {
        case UP:
            menu_to_handle->selected_index = (menu_to_handle->selected_index - 1 + menu_to_handle->option_count) % menu_to_handle->option_count;
            break;
        case DOWN:
            menu_to_handle->selected_index = (menu_to_handle->selected_index + 1) % menu_to_handle->option_count;
            break;
        case ENTER:
            return menu_to_handle->selected_index;
        case ESCAPE:
            return -1;// return -1 to indicate that the menu was closed
        case QUIT:
            return -2;// return -2 to indicate that the game should exit
        default:
            break;// do nothing
    }
    return menu_to_handle->option_count;// return the number of options to indicate that the input was not handled
}
