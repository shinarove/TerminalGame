#include "lvl_up_mode.h"

#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../io/output/specific/character_output.h"
#include "../../logger/logger.h"

#define LVLUP_Y_POS_TITLE 2
#define LVLUP_Y_POS_PLAYER_HEAD 4
#define LVLUP_Y_POS_BODY 9

typedef enum {
    LVL_UP_SELECTION,
    WAIT_AFTER_LVL_UP
} lvl_up_mode_state_t;

enum lvl_up_mode_str_idx {
    // strings that are updated via local
    LEVEL_UP_TITLE,
    CHOOSE_ATTRIBUTE_TEXT,
    STRENGTH_STR,
    INTELLIGENCE_STR,
    AGILITY_STR,
    CONSTITUTION_STR,
    LUCK_STR,
    CONTINUE_TEXT,
    MAX_LVL_UP_STRINGS
};

char** lvl_up_mode_strings = NULL;

Menu* lvl_up_menu = NULL;

lvl_up_mode_state_t lvl_up_state = LVL_UP_SELECTION;

void update_lvl_up_mode_local(void);

int init_lvl_up_mode(void) {
    lvl_up_mode_strings = (char**) malloc(sizeof(char*) * MAX_LVL_UP_STRINGS);
    RETURN_WHEN_NULL(lvl_up_mode_strings, 1, "Level Up Mode", "Failed to allocate memory for level up mode strings.")

    for (int i = 0; i < MAX_LVL_UP_STRINGS; i++) {
        lvl_up_mode_strings[i] = NULL;
    }

    lvl_up_menu = init_simple_menu(NULL, &lvl_up_mode_strings[STRENGTH_STR], MAX_ATTRIBUTES, " ");

    update_lvl_up_mode_local();
    observe_local(update_lvl_up_mode_local);
    return 0;
}

state_t update_lvl_up_mode(const input_t input, Character* player) {
    RETURN_WHEN_NULL(player, EXIT_GAME, "Level Up Mode", "In `update_lvl_up_mode` given player is NULL.")

    state_t res = LVL_UP_MODE;

    print_text(5, LVLUP_Y_POS_TITLE, WHITE, DEFAULT, lvl_up_mode_strings[LEVEL_UP_TITLE]);

    const output_args_c_t lvl_up_args = {0, RES_CURR_MAX, ATTR_MAX};
    print_char_h(5, LVLUP_Y_POS_PLAYER_HEAD, player, lvl_up_args);

    if (lvl_up_state == LVL_UP_SELECTION) {
        switch (lvl_up_menu->vtable->handle_menu(lvl_up_menu, input, 5, LVLUP_Y_POS_BODY)) {
            case STRENGTH:
                player->vtable->lvl_up(player, STRENGTH);
                lvl_up_state = WAIT_AFTER_LVL_UP;
                clear_screen();
                break;
            case INTELLIGENCE:
                player->vtable->lvl_up(player, INTELLIGENCE);
                lvl_up_state = WAIT_AFTER_LVL_UP;
                clear_screen();
                break;
            case AGILITY:
                player->vtable->lvl_up(player, AGILITY);
                lvl_up_state = WAIT_AFTER_LVL_UP;
                clear_screen();
                break;
            case CONSTITUTION:
                player->vtable->lvl_up(player, CONSTITUTION);
                lvl_up_state = WAIT_AFTER_LVL_UP;
                clear_screen();
                break;
            case LUCK:
                player->vtable->lvl_up(player, LUCK);
                lvl_up_state = WAIT_AFTER_LVL_UP;
                clear_screen();
                break;
            case MAX_ATTRIBUTES:// nothing was pressed, do nothing
            case -1:            // ESC was pressed, do nothing
                break;
            case -2:// Ctrl + C was pressed
                res = EXIT_GAME;
                break;
            default:
                break;
        }
    } else if (lvl_up_state == WAIT_AFTER_LVL_UP) {
        print_text(5, LVLUP_Y_POS_BODY, WHITE, DEFAULT, lvl_up_mode_strings[CONTINUE_TEXT]);

        if (input == ENTER) {
            // reset the menu selected index
            lvl_up_menu->selected_index = 0;
            lvl_up_state = LVL_UP_SELECTION;

            res = MAP_MODE;
            clear_screen();
        }
    }

    return res;
}

void shutdown_lvl_up_mode(void) {
    if (lvl_up_mode_strings != NULL) {
        for (int i = 0; i < MAX_LVL_UP_STRINGS; i++) {
            if (lvl_up_mode_strings[i] != NULL) free(lvl_up_mode_strings[i]);
        }
        free(lvl_up_mode_strings);
    }

    destroy_menu(lvl_up_menu);
    lvl_up_menu = NULL;
}

void update_lvl_up_mode_local(void) {
    // free the previous local strings
    for (int i = 0; i < MAX_LVL_UP_STRINGS; i++) {
        if (lvl_up_mode_strings[i] != NULL) {
            free(lvl_up_mode_strings[i]);
        }
    }

    lvl_up_mode_strings[LEVEL_UP_TITLE] = get_local_string("LVL_UP.TITLE");
    lvl_up_mode_strings[CHOOSE_ATTRIBUTE_TEXT] = get_local_string("LVL_UP.MENU.TITLE");
    lvl_up_menu->title = lvl_up_mode_strings[CHOOSE_ATTRIBUTE_TEXT];

    lvl_up_mode_strings[STRENGTH_STR] = get_local_string("STRENGTH");
    lvl_up_mode_strings[INTELLIGENCE_STR] = get_local_string("INTELLIGENCE");
    lvl_up_mode_strings[AGILITY_STR] = get_local_string("AGILITY");
    lvl_up_mode_strings[CONSTITUTION_STR] = get_local_string("CONSTITUTION");
    lvl_up_mode_strings[LUCK_STR] = get_local_string("LUCK");

    lvl_up_mode_strings[CONTINUE_TEXT] = get_local_string("PRESS_ENTER.CONTINUE");
}
