#include "title_screen_mode.h"

#include "../../io/colors.h"
#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../io/output/common/common_output.h"
#include "../../logger/logger.h"

#define MAX_TISC_OPTION 4

enum title_screen_index {
    GAME_TITEL,
    OPTION_NEW_GAME,
    OPTION_LOAD_GAME,
    OPTION_CHANGE_LANGUAGE,
    OPTION_EXIT_GAME,
    MAX_TITLE_SCREEN_STRINGS
};

char** title_screen_strings = NULL;

Menu* title_screen_menu = NULL;

void update_title_screen_local(void);

int init_title_screen() {
    // allocate memory for the local strings
    title_screen_strings = (char**) malloc(sizeof(char*) * MAX_TITLE_SCREEN_STRINGS);
    RETURN_WHEN_NULL(title_screen_strings, 1, "Title Screen Mode", "Failed to allocate memory for title screen strings.");

    for (int i = 0; i < MAX_TITLE_SCREEN_STRINGS; i++) {
        title_screen_strings[i] = NULL;
    }

    title_screen_menu = init_simple_menu(" ", &title_screen_strings[OPTION_NEW_GAME], MAX_TISC_OPTION, " ");

    update_title_screen_local();
    observe_local(update_title_screen_local);
    return 0;
}

state_t update_title_screen(const input_t input) {
    RETURN_WHEN_NULL(title_screen_strings, EXIT_GAME, "Title Screen Mode", "Title screen mode was not initialized.");
    state_t next_state = TITLE_SCREEN;

    // clear_screen();
    print_text(5, 2, color_mapping[RED].value, color_mapping[DEFAULT].key, title_screen_strings[GAME_TITEL]);

    switch (title_screen_menu->vtable->handle_menu(title_screen_menu, input, 5, 4)) {
        case 0:// New game was selected
            next_state = CHARACTER_CREATION;
            clear_screen();
            break;
        case 1:// Load game was selected
            next_state = LOAD_GAME;
            clear_screen();
            break;
        case 2:// Change language was selected
            next_state = LANGUAGE_MODE;
            clear_screen();
            break;
        case 3:// Exit game was selected
            next_state = EXIT_GAME;
            break;
        case -1:// ESC was pressed
            break;
        case -2:
            next_state = EXIT_GAME;
            break;
        case MAX_TISC_OPTION://does nothing
            break;
        default:
            log_msg(WARNING, "Title Screen Mode", "Invalid option returned in handle_menu: %d",
                title_screen_menu->selected_index);
            break;
    }

    return next_state;
}

void shutdown_title_screen() {
    if (title_screen_strings != NULL) {
        for (int i = 0; i < MAX_TITLE_SCREEN_STRINGS; i++) {
            if (title_screen_strings[i] != NULL) {
                //only free the strings that were allocated
                free(title_screen_strings[i]);
            }
        }
        free(title_screen_strings);
        title_screen_strings = NULL;
    }

    destroy_menu(title_screen_menu);
    title_screen_menu = NULL;
}

void update_title_screen_local() {
    if (title_screen_strings == NULL) return;

    for (int i = 0; i < MAX_TITLE_SCREEN_STRINGS; i++) {
        if (title_screen_strings[i] != NULL) {
            //only free the strings that were allocated
            free(title_screen_strings[i]);
        }
    }

    title_screen_strings[GAME_TITEL] = get_local_string("GAME.TITLE");
    title_screen_strings[OPTION_NEW_GAME] = get_local_string("NEW_GAME");
    title_screen_strings[OPTION_LOAD_GAME] = get_local_string("LOAD_GAME");
    title_screen_strings[OPTION_CHANGE_LANGUAGE] = get_local_string("CHANGE_LANGUAGE");
    title_screen_strings[OPTION_EXIT_GAME] = get_local_string("EXIT_GAME");
}
