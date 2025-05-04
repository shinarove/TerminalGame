#include "change_language_mode.h"

#include "../../io/colors.h"
#include "../../io/menu.h"
#include "../../io/output/common/common_output.h"
#include "../../logger/logger.h"
#include "../../io/local/local_handler.h"

#define MAX_LANGUAGES 2

enum change_language_index {
    GAME_TITEL,
    MENU_TITLE,
    OPTION_ENGLISH,
    OPTION_GERMAN,
    TAILING_MSG,
    MAX_CHANGE_LANGUAGE_INDEX
};

char** change_language_strings = NULL;

menu_t change_language_menu;

void update_change_language_local(void);

int init_change_language() {
    // allocate memory for the local strings
    change_language_strings = (char**) malloc(sizeof(char*) * MAX_CHANGE_LANGUAGE_INDEX);
    RETURN_WHEN_NULL(change_language_strings, 1, "Change Language Mode", "Failed to allocate memory for change language strings.");

    for (int i = 0; i < MAX_CHANGE_LANGUAGE_INDEX; i++) {
        change_language_strings[i] = NULL;
    }

    change_language_menu.options = &change_language_strings[OPTION_ENGLISH]; // only the first option address is needed
    change_language_menu.option_count = MAX_LANGUAGES;
    change_language_menu.selected_index = 0;

    update_change_language_local();
    observe_local(update_change_language_local);
    return 0;
}

state_t update_change_language(const input_t input, const state_t called_from) {
    RETURN_WHEN_NULL(change_language_strings, EXIT_GAME, "Change Language Mode", "Change language mode was not initialized.");
    state_t next_state = CHANGE_LANGUAGE;

    clear_screen();
    print_text(5, 2, color_mapping[RED].value, color_mapping[DEFAULT].key, change_language_strings[GAME_TITEL]);



    int local_res = 0;
    switch (handle_menu(input, 5, 4, &change_language_menu)) {
        case 0:
            // change language to English
            if (get_language() != LANGE_EN) {
                local_res = set_language(LANGE_EN);
            }
            next_state = local_res == 0 ? called_from : EXIT_GAME;
            break;
        case 1:
            // change language to German
            if (get_language() != LANGE_DE) {
                local_res = set_language(LANGE_DE);
            }
            next_state = local_res == 0 ? called_from : EXIT_GAME;
            break;
        case -1:
            // go back to the previous menu
            next_state = called_from;
            break;
        case -2:
            next_state = EXIT_GAME;
            break;
        case MAX_LANGUAGES:
            // do nothing
            break;
        default:
            log_msg(WARNING, "Change Language Mode", "Invalid option returned in handle_menu: %d", change_language_menu.selected_index);
            break;
    }

    return next_state;
}

void shutdown_change_language() {
    if (change_language_strings == NULL) return;

    for (int i = 0; i < MAX_CHANGE_LANGUAGE_INDEX; i++) {
        if (change_language_strings[i] != NULL) {
            free(change_language_strings[i]);
        }
    }
    free(change_language_strings);
}

void update_change_language_local() {
    if (change_language_strings == NULL) return;

    for (int i = 0; i < MAX_CHANGE_LANGUAGE_INDEX; i++) {
        if (change_language_strings[i] != NULL)  {
            // only free the strings that were allocated
            free(change_language_strings[i]);
        }
    }

    change_language_strings[GAME_TITEL] = get_local_string("GAME.TITLE");
    change_language_strings[MENU_TITLE] = get_local_string("CHANGE_LANGUAGE.TITLE");
    change_language_menu.title = change_language_strings[MENU_TITLE];

    change_language_strings[OPTION_ENGLISH] = get_local_string("ENGLISH");
    change_language_strings[OPTION_GERMAN] = get_local_string("GERMAN");
    change_language_strings[TAILING_MSG] = get_local_string("PRESS_ESC.RETURN");
    change_language_menu.tailing_text = change_language_strings[TAILING_MSG];
}
