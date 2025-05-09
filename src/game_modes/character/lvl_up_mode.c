#include "lvl_up_mode.h"

#include "../../game_data/character/character.h"
#include "../../game_data/character/stats.h"
#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../logger/logger.h"

#define LVLUP_Y_POS_TITLE 2
#define LVLUP_Y_POS_PLAYER_HEAD 4
#define LVLUP_Y_POS_BODY 9

#define CHAR_NAME_LVL_FORMAT "%s | %s: %d"
#define CHAR_RESOURCES_FORMAT "%s: %d/%d | %s: %d/%d | %s: %d/%d"
#define CHAR_ATTRIBUTES_FORMAT "%s: %d | %s: %d | %s: %d | %s: %d | %s: %d"
#define HORIZONTAL_LINE "--------------------------------------------------"
#define LVL_UP_HEAD_FORMAT "%s\n" CHAR_RESOURCES_FORMAT "\n" CHAR_ATTRIBUTES_FORMAT "\n" HORIZONTAL_LINE

typedef enum {
    LVL_UP_SELECTION,
    WAIT_AFTER_LVL_UP
} lvl_up_mode_state_t;

enum lvl_up_mode_index {
    // strings that are updated via local
    LEVEL_UP_TITLE,
    CHOOSE_ATTRIBUTE_TEXT,
    STRENGTH_STR,
    INTELLIGENCE_STR,
    AGILITY_STR,
    ENDURANCE_STR,
    LUCK_STR,
    LEVEL_STR,
    HEALTH_STR,
    STAMINA_STR,
    MANA_STR,
    CONTINUE_TEXT,
    // strings that are prepared in advance
    PLAYER_NAME_LVL,
    // strings that are updated after an action
    LVL_UP_HEAD,
    MAX_LVL_UP_STRINGS
};

char** lvl_up_mode_strings = NULL;

menu_t lvl_up_menu;

lvl_up_mode_state_t lvl_up_state = LVL_UP_SELECTION;

void update_lvl_up_mode_local(void);

void update_lvl_up_head(const character_t* player);

int init_lvl_up_mode(void) {
    lvl_up_mode_strings = (char**) malloc(sizeof(char*) * MAX_LVL_UP_STRINGS);
    RETURN_WHEN_NULL(lvl_up_mode_strings, 1, "Level Up Mode", "Failed to allocate memory for level up mode strings.")

    for (int i = 0; i < MAX_LVL_UP_STRINGS; i++) {
        lvl_up_mode_strings[i] = NULL;
    }

    lvl_up_menu.options = &lvl_up_mode_strings[STRENGTH_STR];
    lvl_up_menu.option_count = MAX_ATTRIBUTES;
    lvl_up_menu.selected_index = 0;
    lvl_up_menu.tailing_text = " ";

    update_lvl_up_mode_local();
    observe_local(update_lvl_up_mode_local);
    return 0;
}

state_t prepare_lvl_up_mode(const character_t* player) {
    RETURN_WHEN_NULL(player, EXIT_GAME, "Level Up Mode", "In `prepare_lvl_up_mode` given player is NULL.");
    // free the previously prepared strings
    for (int i = PLAYER_NAME_LVL; i < LVL_UP_HEAD; i++) {
        if (lvl_up_mode_strings[i] != NULL) {
            free(lvl_up_mode_strings[i]);
        }
    }

    char* buffer = malloc(64);
    snprintf(buffer, 64, CHAR_NAME_LVL_FORMAT,
             player->name, lvl_up_mode_strings[LEVEL_STR], player->level);
    lvl_up_mode_strings[PLAYER_NAME_LVL] = buffer;

    update_lvl_up_head(player);
    return LVL_UP_MODE;
}

state_t update_lvl_up_mode(const input_t input, character_t* player) {
    RETURN_WHEN_NULL(player, EXIT_GAME, "Level Up Mode", "In `update_lvl_up_mode` given player is NULL.");

    state_t res = LVL_UP_MODE;

    print_text(5, LVLUP_Y_POS_TITLE, WHITE, DEFAULT, lvl_up_mode_strings[LEVEL_UP_TITLE]);
    print_text(5, LVLUP_Y_POS_PLAYER_HEAD, WHITE, DEFAULT, lvl_up_mode_strings[LVL_UP_HEAD]);

    if (lvl_up_state == LVL_UP_SELECTION) {
        switch (handle_menu(input, 5, LVLUP_Y_POS_BODY, &lvl_up_menu)) {
            case STRENGTH:
                lvl_up_c(player, STRENGTH);
                lvl_up_state = WAIT_AFTER_LVL_UP;
                update_lvl_up_head(player);
                clear_screen();
                break;
            case INTELLIGENCE:
                lvl_up_c(player, INTELLIGENCE);
                lvl_up_state = WAIT_AFTER_LVL_UP;
                update_lvl_up_head(player);
                clear_screen();
                break;
            case AGILITY:
                lvl_up_c(player, AGILITY);
                lvl_up_state = WAIT_AFTER_LVL_UP;
                update_lvl_up_head(player);
                clear_screen();
                break;
            case ENDURANCE:
                lvl_up_c(player, ENDURANCE);
                lvl_up_state = WAIT_AFTER_LVL_UP;
                update_lvl_up_head(player);
                clear_screen();
                break;
            case LUCK:
                lvl_up_c(player, LUCK);
                lvl_up_state = WAIT_AFTER_LVL_UP;
                update_lvl_up_head(player);
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
            res = MAP_MODE;
            clear_screen();
        }
    }

    return res;
}

void free_prepared_lum_resources(void) {
    for (int i = PLAYER_NAME_LVL; i < LVL_UP_HEAD; i++) {
        if (lvl_up_mode_strings[i] != NULL) {
            free(lvl_up_mode_strings[i]);
            lvl_up_mode_strings[i] = NULL;
        }
    }
}

void shutdown_lvl_up_mode(void) {
    if (lvl_up_mode_strings != NULL) {
        for (int i = 0; i < MAX_LVL_UP_STRINGS; i++) {
            if (lvl_up_mode_strings[i] != NULL) free(lvl_up_mode_strings[i]);
        }
        free(lvl_up_mode_strings);
    }
}

void update_lvl_up_mode_local(void) {
    // free the previous local strings
    for (int i = 0; i < LVL_UP_HEAD; i++) {
        if (lvl_up_mode_strings[i] != NULL) {
            free(lvl_up_mode_strings[i]);
        }
    }

    lvl_up_mode_strings[LEVEL_UP_TITLE] = get_local_string("LEVEL_UP.TITLE");
    lvl_up_mode_strings[CHOOSE_ATTRIBUTE_TEXT] = get_local_string("LVL_UP.MENU.TITLE");
    lvl_up_menu.title = lvl_up_mode_strings[CHOOSE_ATTRIBUTE_TEXT];

    lvl_up_mode_strings[STRENGTH_STR] = get_local_string("STRENGTH");
    lvl_up_mode_strings[INTELLIGENCE_STR] = get_local_string("INTELLIGENCE");
    lvl_up_mode_strings[AGILITY_STR] = get_local_string("AGILITY");
    lvl_up_mode_strings[ENDURANCE_STR] = get_local_string("ENDURANCE");
    lvl_up_mode_strings[LUCK_STR] = get_local_string("LUCK");
    lvl_up_mode_strings[LEVEL_STR] = get_local_string("LEVEL");
    lvl_up_mode_strings[HEALTH_STR] = get_local_string("HEALTH");
    lvl_up_mode_strings[STAMINA_STR] = get_local_string("STAMINA");
    lvl_up_mode_strings[MANA_STR] = get_local_string("MANA");

    lvl_up_mode_strings[CONTINUE_TEXT] = get_local_string("PRESS_ENTER.CONTINUE");
}

void update_lvl_up_head(const character_t* player) {
    // free the previous head string
    if (lvl_up_mode_strings[LVL_UP_HEAD] != NULL) {
        free(lvl_up_mode_strings[LVL_UP_HEAD]);
    }

    // update the level up head string
    char* lvl_up_head_str = malloc(256);
    snprintf(lvl_up_head_str, 256, LVL_UP_HEAD_FORMAT,
             lvl_up_mode_strings[PLAYER_NAME_LVL],
             lvl_up_mode_strings[HEALTH_STR], player->current_resources.health, player->max_resources.health,
             lvl_up_mode_strings[STAMINA_STR], player->current_resources.stamina, player->max_resources.stamina,
             lvl_up_mode_strings[MANA_STR], player->current_resources.mana, player->max_resources.mana,
             lvl_up_mode_strings[STRENGTH_STR], player->current_attributes.strength,
             lvl_up_mode_strings[INTELLIGENCE_STR], player->current_attributes.intelligence,
             lvl_up_mode_strings[AGILITY_STR], player->current_attributes.agility,
             lvl_up_mode_strings[ENDURANCE_STR], player->current_attributes.endurance,
             lvl_up_mode_strings[LUCK_STR], player->current_attributes.luck);
    lvl_up_mode_strings[LVL_UP_HEAD] = lvl_up_head_str;
}
