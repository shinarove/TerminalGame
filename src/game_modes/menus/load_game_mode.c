#include "load_game_mode.h"

#include "../../game_data/save_file_handler.h"
#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../logger/logger.h"

typedef enum {
    SELECT_SLOT,// only when at least one save file is found
    CONFIRM_DISCARD,
    SHOW_NO_SAVES,
    SHOW_SUCCESS,
    SHOW_FAILURE
} load_game_state_t;

enum load_game_str_idx {
    // strings that are updated via local
    CHOOSE_SAVE_SLOT,
    SAVE_SLOT_1,
    SAVE_SLOT_2,
    SAVE_SLOT_3,
    SAVE_SLOT_4,
    SAVE_SLOT_5,
    NO_SAVES_FOUND,
    DISCARD_CURRENT_GAME,
    SUCCESS_TEXT,
    FAILURE_TEXT,
    RETURN_TEXT,
    CONTINUE_TEXT,
    CONFIRM_TEXT,
    // strings that are prepared
    SAVE_SLOT_A_FULL,// these slots are not associated with a numeric save slot
    SAVE_SLOT_B_FULL,
    SAVE_SLOT_C_FULL,
    SAVE_SLOT_D_FULL,
    SAVE_SLOT_E_FULL,
    MAX_LOAD_GAME_STRINGS,
};

char** load_game_mode_strings = NULL;

memory_pool_t* pool_to_load_in = NULL;
game_state_t* game_state_to_load = NULL;

Menu* load_game_menu = NULL;

// 1 for an empty slot / 0 for not empty
int empty_save_slot_l[MAX_SAVE_SLOTS] = {1, 1, 1, 1, 1};
int slot_to_load = 0;

load_game_state_t lg_state = SELECT_SLOT;

void update_load_game_local(void);

int check_game_state(const game_state_t* game_state);

void load_helper(save_slot_t slot, const memory_pool_t* pool, game_state_t* game_state);

int init_load_game_mode() {
    load_game_mode_strings = (char**) malloc(sizeof(char*) * MAX_LOAD_GAME_STRINGS);
    RETURN_WHEN_NULL(load_game_mode_strings, 1, "Load Game Mode", "Failed to allocate memory for load game mode strings.")

    for (int i = 0; i < MAX_LOAD_GAME_STRINGS; i++) {
        load_game_mode_strings[i] = NULL;
    }

    load_game_menu = init_simple_menu(NULL, &load_game_mode_strings[CHOOSE_SAVE_SLOT], 0, NULL);

    update_load_game_local();
    observe_local(update_load_game_local);
    return 0;
}

state_t prepare_load_game_mode(memory_pool_t* pool, game_state_t* game_state) {
    RETURN_WHEN_NULL(pool, EXIT_GAME,
                     "Load Game Mode", "In `prepare_load_game_mode` given memory pool is NULL.")
    if (check_game_state(game_state) != 0) return EXIT_GAME;

    lg_state = SELECT_SLOT;
    state_t res = LOAD_GAME;
    load_game_menu->selected_index = 0;
    slot_to_load = 0;

    // prepare slot strings with save slot infos
    const save_infos_t infos = get_save_infos();
    if (infos.length == 5) {
        int active_slot_count = 0;
        for (int i = 0; i < infos.length; i++) {
            if (load_game_mode_strings[i + SAVE_SLOT_A_FULL] != NULL) {
                free(load_game_mode_strings[i + SAVE_SLOT_A_FULL]);
                load_game_mode_strings[i + SAVE_SLOT_A_FULL] = NULL;
            }

            if (infos.dates[i] != NULL) {
                load_game_mode_strings[active_slot_count + SAVE_SLOT_A_FULL] = malloc(64 * sizeof(char));
                snprintf(load_game_mode_strings[active_slot_count + SAVE_SLOT_A_FULL], 64, "%s [%s]",
                         load_game_mode_strings[i + SAVE_SLOT_1], infos.dates[i]);
                free(infos.dates[i]);

                empty_save_slot_l[i] = 0;
                active_slot_count++;
            } else {
                empty_save_slot_l[i] = 1;
            }
        }
        load_game_menu->option_count = active_slot_count;
        if (active_slot_count == 0) {
            // no save files found
            lg_state = SHOW_NO_SAVES;
            pool_to_load_in = NULL;
            game_state_to_load = NULL;
        } else {
            // at least one save file found
            pool_to_load_in = pool;
            game_state_to_load = game_state;
        }
        free(infos.dates);
    } else {
        log_msg(ERROR, "Load Game Mode", "Save infos length is not 5, but %d", infos.length);
        res = EXIT_GAME;
    }
    return res;
}

state_t update_load_game_mode(const input_t input, const state_t called_from) {
    state_t res = LOAD_GAME;

    switch (lg_state) {
        case CHOOSE_SAVE_SLOT:
            const int selected_index = load_game_menu->vtable->handle_menu(load_game_menu, input, 5, 2);
            if (selected_index >= 0 && selected_index < load_game_menu->option_count) {
                // valid save slot selected
                int real_index = -1;
                int skipped = 0;
                for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
                    if (empty_save_slot_l[i] == 1) continue;
                    if (skipped == selected_index) {
                        real_index = i;
                        break;
                    }
                    skipped++;
                }
                if (called_from == MAIN_MENU) {
                    slot_to_load = real_index;
                    lg_state = CONFIRM_DISCARD;
                    clear_screen();
                } else {
                    load_helper(real_index, pool_to_load_in, game_state_to_load);
                }
            } else if (selected_index == -1) {
                // ESC was pressed
                res = called_from;
                clear_screen();
            } else if (selected_index == -2) {
                res = EXIT_GAME;
            }
            break;
        case CONFIRM_DISCARD:
            print_text(5, 2, WHITE, DEFAULT, load_game_mode_strings[DISCARD_CURRENT_GAME]);
            print_text(5, 4, WHITE, DEFAULT, load_game_mode_strings[CONFIRM_TEXT]);
            print_text(5, 5, WHITE, DEFAULT, load_game_mode_strings[RETURN_TEXT]);
            if (input == ENTER) {
                load_helper(slot_to_load, pool_to_load_in, game_state_to_load);
            } else if (input == ESCAPE) {
                lg_state = SELECT_SLOT;
                clear_screen();
            } else if (input == QUIT) {
                res = EXIT_GAME;
            }
            break;
        case SHOW_NO_SAVES:
            print_text(5, 2, WHITE, DEFAULT, load_game_mode_strings[NO_SAVES_FOUND]);
            print_text(5, 4, WHITE, DEFAULT, load_game_mode_strings[RETURN_TEXT]);
            if (input == ESCAPE) {
                lg_state = SELECT_SLOT;
                clear_screen();
            } else if (input == QUIT) {
                res = EXIT_GAME;
            }
            break;
        case SHOW_SUCCESS:
            print_text(5, 2, WHITE, DEFAULT, load_game_mode_strings[SUCCESS_TEXT]);
            print_text(5, 4, WHITE, DEFAULT, load_game_mode_strings[CONTINUE_TEXT]);
            if (input == ENTER) {
                res = MAP_MODE;
                clear_screen();
            } else if (input == QUIT) {
                res = EXIT_GAME;
            }
            break;
        case SHOW_FAILURE:
            print_text(5, 2, WHITE, DEFAULT, load_game_mode_strings[FAILURE_TEXT]);
            print_text(5, 4, WHITE, DEFAULT, load_game_mode_strings[CONTINUE_TEXT]);
            if (input == ENTER) {
                res = TITLE_SCREEN;
                clear_screen();
            } else if (input == QUIT) {
                res = EXIT_GAME;
            }
            break;
    }

    return res;
}

void shutdown_load_game_mode(void) {
    if (load_game_mode_strings != NULL) {
        for (int i = 0; i < MAX_LOAD_GAME_STRINGS; i++) {
            if (load_game_mode_strings[i] != NULL) {
                free(load_game_mode_strings[i]);
            }
        }
        free(load_game_mode_strings);
        load_game_mode_strings = NULL;
    }

    destroy_menu(load_game_menu);
    load_game_menu = NULL;
}

void update_load_game_local(void) {
    for (int i = 0; i < SAVE_SLOT_A_FULL; i++) {
        if (load_game_mode_strings[i] != NULL) {
            free(load_game_mode_strings[i]);
        }
    }

    load_game_mode_strings[CHOOSE_SAVE_SLOT] = get_local_string("LOAD_GAME.CHOOSE.SLOT");
    load_game_menu->title = load_game_mode_strings[CHOOSE_SAVE_SLOT];
    load_game_mode_strings[SAVE_SLOT_1] = get_local_string("SAVE.SLOT.1");
    load_game_mode_strings[SAVE_SLOT_2] = get_local_string("SAVE.SLOT.2");
    load_game_mode_strings[SAVE_SLOT_3] = get_local_string("SAVE.SLOT.3");
    load_game_mode_strings[SAVE_SLOT_4] = get_local_string("SAVE.SLOT.4");
    load_game_mode_strings[SAVE_SLOT_5] = get_local_string("SAVE.SLOT.5");

    load_game_mode_strings[NO_SAVES_FOUND] = get_local_string("LOAD_GAME.NOT_FOUND");
    load_game_mode_strings[DISCARD_CURRENT_GAME] = get_local_string("LOAD_GAME.DISCARD");
    load_game_mode_strings[SUCCESS_TEXT] = get_local_string("LOAD_GAME.LOAD.SUCCESS");
    load_game_mode_strings[FAILURE_TEXT] = get_local_string("LOAD_GAME.LOAD.FAILURE");

    load_game_mode_strings[RETURN_TEXT] = get_local_string("PRESS_ESC.RETURN");
    load_game_menu->tailing_text = load_game_mode_strings[RETURN_TEXT];
    load_game_mode_strings[CONTINUE_TEXT] = get_local_string("PRESS_ENTER.CONTINUE");
    load_game_mode_strings[CONFIRM_TEXT] = get_local_string("PRESS_ENTER.CONFIRM");
}

int check_game_state(const game_state_t* game_state) {
    RETURN_WHEN_NULL(game_state, 1, "Load Game Mode", "Game state is NULL.");
    RETURN_WHEN_NULL(game_state->maps, 1, "Load Game Mode", "Game state maps are NULL.");
    RETURN_WHEN_NULL(game_state->player, 1, "Load Game Mode", "Game state player is NULL.");
    return 0;
}

void load_helper(const save_slot_t slot, const memory_pool_t* pool, game_state_t* game_state) {
    // destroy the previous maps
    for (int i = 0; i < game_state->max_floors; i++) {
        destroy_map(pool, game_state->maps[i]);
        game_state->maps[i] = NULL;
    }

    // destroy previous stored character
    destroy_character(game_state->player);
    game_state->player = NULL;

    if (load_game_state(slot, pool, game_state) == 0) {
        lg_state = SHOW_SUCCESS;
        clear_screen();
    } else {
        lg_state = SHOW_FAILURE;
        clear_screen();
    }
}
