#include "main_menu_mode.h"

#include "../../io/colors.h"
#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../io/output/common/common_output.h"
#include "../../logger/logger.h"

#define MAX_MAIN_MENU_OPTIONS 6

typedef enum {
    MENU_VIEW,
    CONFIRM_DISCARD
} main_menu_state_t;

enum main_menu_index {
    GAME_TITLE,
    OPTION_CONTINUE,
    OPTION_NEW_GAME,
    OPTION_SAVE_GAME,
    OPTION_LOAD_GAME,
    OPTION_CHANGE_LANGUAGE,
    OPTION_EXIT_GAME,
    CONFIRM_DISCARD_MSG,
    ENTER_CONFIRM,
    ESC_RETURN,
    MAX_MAIN_MENU_STRINGS
};

char** main_menu_strings = NULL;

main_menu_state_t main_menu_state = MENU_VIEW;

Menu* main_menu = NULL;

void update_main_menu_local(void);

int init_main_menu() {
    // allocate memory for the local strings
    main_menu_strings = (char**) malloc(sizeof(char*) * MAX_MAIN_MENU_STRINGS);
    RETURN_WHEN_NULL(main_menu_strings, 1, "Main Menu Mode", "Failed to allocate memory for main menu strings.");

    for (int i = 0; i < MAX_MAIN_MENU_STRINGS; i++) {
        main_menu_strings[i] = NULL;
    }

    main_menu = init_simple_menu(" ", &main_menu_strings[OPTION_CONTINUE], MAX_MAIN_MENU_OPTIONS, " ");

    update_main_menu_local();
    observe_local(update_main_menu_local);
    return 0;
}

state_t update_main_menu(const input_t input) {
    RETURN_WHEN_NULL(main_menu_strings, EXIT_GAME, "Main Menu Mode", "Main menu mode was not initialized.");
    state_t next_state = MAIN_MENU;

    switch (main_menu_state) {
        case MENU_VIEW:
            print_text(5, 2, RED, DEFAULT, main_menu_strings[GAME_TITLE]);

            switch (main_menu->vtable->handle_menu(main_menu, input, 5, 4)) {
                case 0:// continue was selected
                    next_state = MAP_MODE;
                    main_menu->selected_index = 0;
                    clear_screen();
                    break;
                case 1:// restart the game
                    main_menu_state = CONFIRM_DISCARD;
                    clear_screen();
                    break;
                case 2:// save game
                    next_state = SAVE_GAME;
                    main_menu->selected_index = 0;
                    clear_screen();
                    break;
                case 3:// load game
                    next_state = LOAD_GAME;
                    main_menu->selected_index = 0;
                    clear_screen();
                    break;
                case 4:// change language
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
                    main_menu->selected_index = 0;
                    clear_screen();
                    break;
                case -2:
                    next_state = EXIT_GAME;
                    break;
                default:
                    log_msg(WARNING, "Main Menu Mode", "Invalid option returned in handle_menu");
                    break;
            }
            break;
        case CONFIRM_DISCARD:
            print_text(5, 2, WHITE, DEFAULT, main_menu_strings[CONFIRM_DISCARD_MSG]);
            print_text(5, 4, WHITE, DEFAULT, main_menu_strings[ENTER_CONFIRM]);
            print_text(5, 5, WHITE, DEFAULT, main_menu_strings[ESC_RETURN]);

            switch (input) {
                case ENTER:
                    next_state = RESTART_GAME;
                    main_menu->selected_index = 0;
                    main_menu_state = MENU_VIEW;
                    clear_screen();
                    break;
                case ESCAPE:
                    main_menu_state = MENU_VIEW;
                    clear_screen();
                    break;
                case QUIT:
                    next_state = EXIT_GAME;
                    break;
                default:;
            }
            break;
    }

    return next_state;
}

void shutdown_main_menu() {
    if (main_menu_strings != NULL) {
        for (int i = 0; i < MAX_MAIN_MENU_STRINGS; i++) {
            if (main_menu_strings[i] != NULL) {
                //only free the strings that were allocated
                free(main_menu_strings[i]);
            }
        }
        free(main_menu_strings);
        main_menu_strings = NULL;
    }

    destroy_menu(main_menu);
    main_menu = NULL;
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
    main_menu_strings[CONFIRM_DISCARD_MSG] = get_local_string("MENU.MAIN.DISCARD");
    main_menu_strings[ENTER_CONFIRM] = get_local_string("PRESS_ENTER.CONFIRM");
    main_menu_strings[ESC_RETURN] = get_local_string("PRESS_ESC.RETURN");
}
