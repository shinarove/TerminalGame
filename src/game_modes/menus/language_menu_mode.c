#include "language_menu_mode.h"

#include "../../io/colors.h"
#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../io/output/common/common_output.h"
#include "../../logger/logger.h"

#define MAX_LANGUAGES 2

enum change_language_str_idx {
    GAME_TITEL,
    MENU_TITLE,
    OPTION_ENGLISH,
    OPTION_GERMAN,
    TAILING_MSG,
    MAX_CHANGE_LANGUAGE_STRINGS
};

char** language_menu_strings = NULL;

Menu* change_language_menu = NULL;

void update_change_language_local(void);

int init_change_language() {
    // allocate memory for the local strings
    language_menu_strings = (char**) malloc(sizeof(char*) * MAX_CHANGE_LANGUAGE_STRINGS);
    RETURN_WHEN_NULL(language_menu_strings, 1, "Change Language Mode", "Failed to allocate memory for change language strings.");

    for (int i = 0; i < MAX_CHANGE_LANGUAGE_STRINGS; i++) {
        language_menu_strings[i] = NULL;
    }

    change_language_menu = init_simple_menu(NULL, &language_menu_strings[OPTION_ENGLISH], MAX_LANGUAGES, NULL);

    update_change_language_local();
    observe_local(update_change_language_local);
    return 0;
}

state_t update_change_language(const input_t input, const state_t called_from) {
    RETURN_WHEN_NULL(language_menu_strings, EXIT_GAME, "Change Language Mode", "Change language mode was not initialized.");
    state_t next_state = LANGUAGE_MODE;

    // clear_screen();
    print_text(5, 2, color_mapping[RED].value, color_mapping[DEFAULT].key, language_menu_strings[GAME_TITEL]);

    int local_res = 0;
    switch (change_language_menu->vtable->handle_menu(change_language_menu, input, 5, 4)) {
        case 0:
            // change language to English
            if (get_language() != LANGE_EN) {
                local_res = set_language(LANGE_EN);
            }
            change_language_menu->selected_index = 0;
            clear_screen();
            next_state = local_res == 0 ? called_from : EXIT_GAME;
            break;
        case 1:
            // change language to German
            if (get_language() != LANGE_DE) {
                local_res = set_language(LANGE_DE);
            }
            change_language_menu->selected_index = 0;
            clear_screen();
            next_state = local_res == 0 ? called_from : EXIT_GAME;
            break;
        case -1:
            // go back to the previous menu
            change_language_menu->selected_index = 0;
            clear_screen();
            next_state = called_from;
            break;
        case -2:
            next_state = EXIT_GAME;
            break;
        case MAX_LANGUAGES:
            // do nothing
            break;
        default:
            log_msg(WARNING, "Change Language Mode", "Invalid option returned in handle_menu: %d",
                change_language_menu->selected_index);
            break;
    }

    return next_state;
}

void shutdown_change_language() {
    if (language_menu_strings != NULL) {
        for (int i = 0; i < MAX_CHANGE_LANGUAGE_STRINGS; i++) {
            if (language_menu_strings[i] != NULL) {
                free(language_menu_strings[i]);
            }
        }
        free(language_menu_strings);
        language_menu_strings = NULL;
    }

    destroy_menu(change_language_menu);
    change_language_menu = NULL;
}

void update_change_language_local() {
    if (language_menu_strings == NULL) return;

    for (int i = 0; i < MAX_CHANGE_LANGUAGE_STRINGS; i++) {
        if (language_menu_strings[i] != NULL) {
            // only free the strings that were allocated
            free(language_menu_strings[i]);
        }
    }

    language_menu_strings[GAME_TITEL] = get_local_string("GAME.TITLE");
    language_menu_strings[MENU_TITLE] = get_local_string("CHANGE_LANGUAGE.TITLE");
    change_language_menu->title = language_menu_strings[MENU_TITLE];

    language_menu_strings[OPTION_ENGLISH] = get_local_string("ENGLISH");
    language_menu_strings[OPTION_GERMAN] = get_local_string("GERMAN");
    language_menu_strings[TAILING_MSG] = get_local_string("PRESS_ESC.RETURN");
    change_language_menu->tailing_text = language_menu_strings[TAILING_MSG];
}
