#include "title_screen_mode.h"

#include "../../io/menu.h"
#include "../../io/colors.h"
#include "../../io/output/common/common_output.h"
#include "../../logger/logger.h"

static const char* menu_options[] = {
    "New Game",
    "Load Game",
    "Exit"
};

static menu_t title_screen_menu = {
    .title = " ",
    .options = (char**) menu_options,
    .option_count = 3,
    .selected_index = 0,
    .tailing_text = " "
};

state_t update_title_screen(input_t input) {
    state_t next_state = TITLE_SCREEN;

    clear_screen();

    print_text(5, 2, color_mapping[RED].value, color_mapping[DEFAULT].key, "Terminal Game");
    switch (handle_menu(input, 5, 5, &title_screen_menu)) {
        case 0:
            next_state = MAP_GENERATION;
            break;
        case 1:
            //TODO: add load functionality
            break;
        case 2:
            next_state = EXIT_GAME;
            break;
        case -1:
            //ESC was pressed, but we are in the title screen, so nothing will change
        case 3: // count of options
            //does nothing
            break;
        default:
            log_msg(WARNING, "Title Screen Mode", "Invalid option returned in handle_menu: %d", title_screen_menu.selected_index);
            break;
    }

    return next_state;
}
