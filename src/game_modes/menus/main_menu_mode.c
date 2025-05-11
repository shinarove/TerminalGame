#include "main_menu_mode.h"

#include "../../io/colors.h"
#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../io/output/common/common_output.h"
#include "../../logger/logger.h"

#define MAX_MAIN_MENU_OPTIONS 6

enum main_menu_index {
    GAME_TITLE,
    OPTION_CONTINUE,
    OPTION_NEW_GAME,
    OPTION_SAVE_GAME,
    OPTION_LOAD_GAME,
    OPTION_CHANGE_LANGUAGE,
    OPTION_EXIT_GAME,
    MAX_MAIN_MENU_STRINGS
};

char** main_menu_strings = NULL;

menu_t main_menu;

void update_main_menu_local(void);

int init_main_menu() {
    // allocate memory for the local strings
    main_menu_strings = (char**) malloc(sizeof(char*) * MAX_MAIN_MENU_STRINGS);
    RETURN_WHEN_NULL(main_menu_strings, 1, "Main Menu Mode", "Failed to allocate memory for main menu strings.");

    for (int i = 0; i < MAX_MAIN_MENU_STRINGS; i++) {
        main_menu_strings[i] = NULL;
    }

    main_menu.title = " ";
    main_menu.options = &main_menu_strings[OPTION_CONTINUE];// only the first option address is needed
    main_menu.option_count = MAX_MAIN_MENU_OPTIONS;
    main_menu.selected_index = 0;
    main_menu.tailing_text = " ";
    main_menu.args = NULL;

    update_main_menu_local();
    observe_local(update_main_menu_local);
    return 0;
}

state_t update_main_menu(const input_t input) {
    RETURN_WHEN_NULL(main_menu_strings, EXIT_GAME, "Main Menu Mode", "Main menu mode was not initialized.");
    state_t next_state = MAIN_MENU;

    print_text(5, 2, color_mapping[RED].value, color_mapping[DEFAULT].key, main_menu_strings[GAME_TITLE]);

    const int res = handle_simple_menu(input, 5, 4, &main_menu);
    switch (res) {
        case 0:// continue was selected
            next_state = MAP_MODE;
            main_menu.selected_index = 0;
            clear_screen();
            break;
        case 1: // restart the game
            next_state = RESTART_GAME;
            main_menu.selected_index = 0;
            clear_screen();
            break;
        case 2:
            next_state = SAVE_GAME;
            main_menu.selected_index = 0;
            clear_screen();
        case 3:
            //TODO: add load functionality
            break;
        case 4:
            next_state = LANGUAGE_MODE;
            clear_screen();
            break;
        case 5:// Exit game was selected
            next_state = EXIT_GAME;
            break;
        case MAX_MAIN_MENU_OPTIONS:// nothing has changed
            break;
        case -1:// Esc was pressed, return to map mode
            next_state = MAP_MODE;
            main_menu.selected_index = 0;
            clear_screen();
            break;
        case -2:
            next_state = EXIT_GAME;
            break;
        default:
            log_msg(WARNING, "Main Menu Mode", "Invalid option returned in handle_menu: %d", res);
            break;
    }

    return next_state;
}

void shutdown_main_menu() {
    if (main_menu_strings == NULL) return;

    for (int i = 0; i < MAX_MAIN_MENU_STRINGS; i++) {
        if (main_menu_strings[i] != NULL) {
            //only free the strings that were allocated
            free(main_menu_strings[i]);
        }
    }
    free(main_menu_strings);
}

void update_main_menu_local() {
    if (main_menu_strings == NULL) return;

    for (int i = 0; i < MAX_MAIN_MENU_STRINGS; i++) {
        if (main_menu_strings[i] != NULL) {
            //only free the strings that were allocated
            free(main_menu_strings[i]);
        }
    }

    main_menu_strings[GAME_TITLE] = get_local_string("GAME.TITLE");
    main_menu_strings[OPTION_CONTINUE] = get_local_string("CONTINUE");
    main_menu_strings[OPTION_NEW_GAME] = get_local_string("NEW_GAME");
    main_menu_strings[OPTION_SAVE_GAME] = get_local_string("SAVE_GAME");
    main_menu_strings[OPTION_LOAD_GAME] = get_local_string("LOAD_GAME");
    main_menu_strings[OPTION_CHANGE_LANGUAGE] = get_local_string("CHANGE_LANGUAGE");
    main_menu_strings[OPTION_EXIT_GAME] = get_local_string("EXIT_GAME");
}
