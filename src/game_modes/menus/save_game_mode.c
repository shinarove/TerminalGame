#include "save_game_mode.h"

#include "../../game_data/save_file_handler.h"
#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../logger/logger.h"

typedef enum {
    SELECT_SLOT,
    CONFIRM_OVERWRITE,
    SHOW_SUCCESS,
    SHOW_FAILURE
} save_game_state_t;

enum save_game_index {
    // strings that are updated via local
    CHOOSE_SAVE_SLOT,
    SAVE_SLOT_1,
    SAVE_SLOT_2,
    SAVE_SLOT_3,
    SAVE_SLOT_4,
    SAVE_SLOT_5,
    EMPTY_STR,
    OVERWRITE_INFO_TEXT,
    SUCCESS_TEXT,
    FAILURE_TEXT,
    RETURN_TEXT,
    CONFIRM_TEXT,
    CONTINUE_TEXT,
    // strings that are prepared
    SAVE_SLOT_1_FULL,
    SAVE_SLOT_2_FULL,
    SAVE_SLOT_3_FULL,
    SAVE_SLOT_4_FULL,
    SAVE_SLOT_5_FULL,
    MAX_SAVE_GAME_STRINGS
};

char** save_game_mode_strings = NULL;

game_state_t* game_state_to_save = NULL;

Menu* save_game_menu = NULL;

// 1 for an empty slot / 0 for not empty
int empty_save_slot_s[MAX_SAVE_SLOTS] = {1, 1, 1, 1, 1};
int slot_to_save = 0;

save_game_state_t sg_state = SELECT_SLOT;

void update_save_game_local(void);

void save_helper(save_slot_t slot, const game_state_t* game_state);

int init_save_game_mode(void) {
    save_game_mode_strings = (char**) malloc(sizeof(char*) * MAX_SAVE_GAME_STRINGS);
    RETURN_WHEN_NULL(save_game_mode_strings, 1, "Save Game Mode", "Failed to allocate memory for save game mode strings.");

    for (int i = 0; i < MAX_SAVE_GAME_STRINGS; i++) {
        save_game_mode_strings[i] = NULL;
    }

    save_game_menu = init_simple_menu(NULL, &save_game_mode_strings[CHOOSE_SAVE_SLOT], MAX_SAVE_SLOTS, NULL);

    update_save_game_local();
    observe_local(update_save_game_local);
    return 0;
}

state_t prepare_save_game_mode(game_state_t* game_state) {
    state_t res = SAVE_GAME;
    sg_state = SELECT_SLOT;
    save_game_menu->selected_index = 0;
    slot_to_save = 0;

    // prepare slot strings with save slot infos
    const save_infos_t infos = get_save_infos();
    if (infos.length == 5) {
        for (int i = 0; i < infos.length; i++) {
            if (save_game_mode_strings[i + SAVE_SLOT_1_FULL] != NULL) {
                free(save_game_mode_strings[i + SAVE_SLOT_1_FULL]);
            }
            save_game_mode_strings[i + SAVE_SLOT_1_FULL] = malloc(64 * sizeof(char));

            if (infos.dates[i] != NULL) {
                empty_save_slot_s[i] = 0;
                snprintf(save_game_mode_strings[i + SAVE_SLOT_1_FULL], 64, "%s [%s]",
                         save_game_mode_strings[i + SAVE_SLOT_1], infos.dates[i]);
                free(infos.dates[i]);
            } else {
                empty_save_slot_s[i] = 1;
                snprintf(save_game_mode_strings[i + SAVE_SLOT_1_FULL], 64, "%s [%s]",
                         save_game_mode_strings[i + SAVE_SLOT_1], save_game_mode_strings[EMPTY_STR]);
            }
        }
        free(infos.dates);
        game_state_to_save = game_state;
    } else {
        log_msg(ERROR, "Save Game Mode", "Save infos length is not 5, but %d", infos.length);
        res = EXIT_GAME;
    }

    return res;
}

state_t update_save_game_mode(const input_t input) {
    state_t res = SAVE_GAME;

    switch (sg_state) {
        case CHOOSE_SAVE_SLOT:
            switch (save_game_menu->vtable->handle_menu(save_game_menu, input, 5, 2)) {
                case 0:
                    save_helper(SLOT_1, game_state_to_save);
                    break;
                case 1:
                    save_helper(SLOT_2, game_state_to_save);
                    break;
                case 2:
                    save_helper(SLOT_3, game_state_to_save);
                    break;
                case 3:
                    save_helper(SLOT_4, game_state_to_save);
                    break;
                case 4:
                    save_helper(SLOT_5, game_state_to_save);
                    break;
                case MAX_SAVE_SLOTS:// nothing was pressed
                    break;
                case -1:// esc was pressed
                    res = MAIN_MENU;
                    clear_screen();
                    break;
                case -2:// ctrl + c was pressed
                    res = EXIT_GAME;
                    break;
                default:
                    break;
            }
            break;
        case CONFIRM_OVERWRITE:
            print_text(5, 2, WHITE, DEFAULT, save_game_mode_strings[OVERWRITE_INFO_TEXT]);
            print_text(5, 4, WHITE, DEFAULT, save_game_mode_strings[CONFIRM_TEXT]);
            print_text(5, 5, WHITE, DEFAULT, save_game_mode_strings[RETURN_TEXT]);
            if (input == ENTER) {
                if (save_game_state(slot_to_save, game_state_to_save) == 0) {
                    sg_state = SHOW_SUCCESS;
                    clear_screen();
                } else {
                    sg_state = SHOW_FAILURE;
                    clear_screen();
                }
            } else if (input == ESCAPE) {
                sg_state = SELECT_SLOT;
                clear_screen();
            } else if (input == QUIT) {
                res = EXIT_GAME;
            }
            break;
        case SHOW_SUCCESS:
            print_text(5, 2, WHITE, DEFAULT, save_game_mode_strings[SUCCESS_TEXT]);
            print_text(5, 4, WHITE, DEFAULT, save_game_mode_strings[CONTINUE_TEXT]);
            if (input == ENTER) {
                res = MAIN_MENU;
                clear_screen();
            } else if (input == ESCAPE) {
                sg_state = SELECT_SLOT;
                clear_screen();
            } else if (input == QUIT) {
                res = EXIT_GAME;
            }
            break;
        case SHOW_FAILURE:
            print_text(5, 2, WHITE, DEFAULT, save_game_mode_strings[FAILURE_TEXT]);
            print_text(5, 4, WHITE, DEFAULT, save_game_mode_strings[CONTINUE_TEXT]);
            if (input == ENTER) {
                res = MAIN_MENU;
                clear_screen();
            } else if (input == ESCAPE) {
                sg_state = SELECT_SLOT;
                clear_screen();
            } else if (input == QUIT) {
                res = EXIT_GAME;
            }
            break;
    }

    return res;
}

void shutdown_save_game_mode(void) {
    if (save_game_mode_strings != NULL) {
        for (int i = 0; i < MAX_SAVE_GAME_STRINGS; i++) {
            if (save_game_mode_strings[i] != NULL) free(save_game_mode_strings[i]);
        }
        free(save_game_mode_strings);
        save_game_mode_strings = NULL;
    }

    destroy_menu(save_game_menu);
    save_game_menu = NULL;
}

void update_save_game_local(void) {
    for (int i = 0; i < SAVE_SLOT_1_FULL; i++) {
        if (save_game_mode_strings[i] != NULL) {
            free(save_game_mode_strings[i]);
        }
    }

    save_game_mode_strings[CHOOSE_SAVE_SLOT] = get_local_string("SAVE_GAME.CHOOSE.SLOT");
    save_game_menu->title = save_game_mode_strings[CHOOSE_SAVE_SLOT];
    save_game_mode_strings[SAVE_SLOT_1] = get_local_string("SAVE.SLOT.1");
    save_game_mode_strings[SAVE_SLOT_2] = get_local_string("SAVE.SLOT.2");
    save_game_mode_strings[SAVE_SLOT_3] = get_local_string("SAVE.SLOT.3");
    save_game_mode_strings[SAVE_SLOT_4] = get_local_string("SAVE.SLOT.4");
    save_game_mode_strings[SAVE_SLOT_5] = get_local_string("SAVE.SLOT.5");
    save_game_mode_strings[EMPTY_STR] = get_local_string("SAVE.SLOT.EMPTY");
    save_game_mode_strings[OVERWRITE_INFO_TEXT] = get_local_string("SAVE_GAME.OVERWRITE.INFO");
    save_game_mode_strings[SUCCESS_TEXT] = get_local_string("SAVE_GAME.SAVE.SUCCESS");
    save_game_mode_strings[FAILURE_TEXT] = get_local_string("SAVE_GAME.SAVE.FAILURE");

    save_game_mode_strings[RETURN_TEXT] = get_local_string("PRESS_ESC.RETURN");
    save_game_menu->tailing_text = save_game_mode_strings[RETURN_TEXT];
    save_game_mode_strings[CONFIRM_TEXT] = get_local_string("PRESS_ENTER.CONFIRM");
    save_game_mode_strings[CONTINUE_TEXT] = get_local_string("PRESS_ENTER.CONTINUE");
}

void save_helper(const save_slot_t slot, const game_state_t* game_state) {
    if (empty_save_slot_s[slot] == 0) {
        sg_state = CONFIRM_OVERWRITE;
        slot_to_save = slot;
        clear_screen();
    } else {
        if (save_game_state(slot, game_state) == 0) {
            sg_state = SHOW_SUCCESS;
            clear_screen();
        } else {
            sg_state = SHOW_FAILURE;
            clear_screen();
        }
    }
}
