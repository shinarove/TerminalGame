#include "menu.h"

#include "../logger/logger.h"

int handle_simple_menu(const input_t input, const int x, const int y, menu_t* menu_to_handle) {
    RETURN_WHEN_NULL(menu_to_handle, -2, "Menu", "Menu to handle is NULL");

    print_simple_menu(x, y, menu_to_handle);

    if (menu_to_handle->args->mode != ACTIVE) {
        // menu is not active, do nothing
        return menu_to_handle->option_count;
    }
    if (menu_to_handle->option_count == 0) {
        // menu has no options, do nothing
        return menu_to_handle->option_count;
    }

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

int handle_spinner_menu(const input_t input, const int x, const int y, const spinner_menu_t* menu_to_handle) {
    RETURN_WHEN_NULL(menu_to_handle, -2, "Menu", "Menu to handle is NULL");
    RETURN_WHEN_NULL(menu_to_handle->menu, -2, "Menu", "Additional info to handle is NULL");

    print_spinner_menu(x, y, menu_to_handle);

    const int real_option_count = menu_to_handle->menu->option_count * 2;

    switch (input) {
        case UP:
            menu_to_handle->menu->selected_index = (menu_to_handle->menu->selected_index - 2 + real_option_count) % real_option_count;
            break;
        case DOWN:
            menu_to_handle->menu->selected_index = (menu_to_handle->menu->selected_index + 2) % real_option_count;
            break;
        case LEFT:
            // move the marker to the left if the marker was on the right side
            if (menu_to_handle->menu->selected_index % 2 != 0) menu_to_handle->menu->selected_index--;
            break;
        case RIGHT:
            // move the marker to the right if the marker was on the left side
            if (menu_to_handle->menu->selected_index % 2 == 0) menu_to_handle->menu->selected_index++;
            break;
        case ENTER:
            return menu_to_handle->menu->selected_index;
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
