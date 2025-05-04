#include "title_screen_mode.h"

#include "../../io/colors.h"
#include "../../io/menu.h"
#include "../../io/output/common/common_output.h"
#include "../../logger/logger.h"
#include "../../io/local/local_handler.h"

#define NUM_OF_OPTIONS 4

enum title_screen_index {
    GAME_TITEL,
    OPTION_NEW_GAME,
    OPTION_LOAD_GAME,
    OPTION_CHANGE_LANGUAGE,
    OPTION_EXIT_GAME,
    MAX_TITLE_SCREEN_INDEX
};

char** title_screen_strings = NULL;

menu_t title_screen_menu;

void update_title_screen_local(void);

int init_title_screen() {
    // allocate memory for the local strings
    title_screen_strings = (char**) malloc(sizeof(char*) * MAX_TITLE_SCREEN_INDEX);
    RETURN_WHEN_NULL(title_screen_strings, 1, "Title Screen Mode", "Failed to allocate memory for title screen strings.");

    for (int i = 0; i < MAX_TITLE_SCREEN_INDEX; i++) {
        title_screen_strings[i] = NULL;
    }

    title_screen_menu.title = " ";
    title_screen_menu.options = &title_screen_strings[OPTION_NEW_GAME]; // only the first option address is needed
    title_screen_menu.option_count = NUM_OF_OPTIONS;
    title_screen_menu.selected_index = 0;
    title_screen_menu.tailing_text = " ";

    update_title_screen_local();

    observe_local(update_title_screen_local);
    return 0;
}

state_t update_title_screen(const input_t input) {
    RETURN_WHEN_NULL(title_screen_strings, EXIT_GAME, "Title Screen Mode", "Title screen mode was not initialized.");
    state_t next_state = TITLE_SCREEN;

    clear_screen();
    print_text(5, 2, color_mapping[RED].value, color_mapping[DEFAULT].key, title_screen_strings[GAME_TITEL]);

    switch (handle_menu(input, 5, 5, &title_screen_menu)) {
        case 0:
            next_state = GENERATE_MAP;
            break;
        case 1:
            //TODO: add load functionality
            break;
        case 2:
            //TODO: add change language functionality
            break;
        case 3:
            next_state = EXIT_GAME;
            break;
        case -1:
            //ESC was pressed, but we are in the title screen, so nothing will change
        case NUM_OF_OPTIONS:
            //does nothing
            break;
        default:
            log_msg(WARNING, "Title Screen Mode", "Invalid option returned in handle_menu: %d", title_screen_menu.selected_index);
            break;
    }

    return next_state;
}

void shutdown_title_screen() {
    if (title_screen_strings == NULL) return;

    for (int i = 0; i < MAX_TITLE_SCREEN_INDEX; i++) {
        if (title_screen_strings[i] != NULL) {
            //only free the strings that were allocated
            free(title_screen_strings[i]);
        }
    }
    free(title_screen_strings);
}

void update_title_screen_local() {
    if (title_screen_strings == NULL) return;

    for (int i = 0; i < MAX_TITLE_SCREEN_INDEX; i++) {
        if (title_screen_strings[i] != NULL)  {
            //only free the strings that were allocated
            free(title_screen_strings[i]);
        }
    }

    title_screen_strings[GAME_TITEL] = get_local_string("GAME.TITLE");
    title_screen_strings[OPTION_NEW_GAME] = get_local_string("OPTION.NEW_GAME");
    title_screen_strings[OPTION_LOAD_GAME] = get_local_string("OPTION.LOAD_GAME");
    title_screen_strings[OPTION_CHANGE_LANGUAGE] = get_local_string("OPTION.CHANGE_LANGUAGE");
    title_screen_strings[OPTION_EXIT_GAME] = get_local_string("OPTION.EXIT_GAME");
}
