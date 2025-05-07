#include "combat_mode.h"

#include "../../io/local/local_handler.h"
#include "../../io/output/common/common_output.h"
#include "../../logger/logger.h"

#include <stdlib.h>

#define MAX_COMBAT_MAIN_OPTIONS 2

#define CHAR_NAME_LVL_FORMAT "%s | %s: %d"
#define CHAR_RESOURCES_FORMAT "%s: %d/%d | %s: %d/%d | %s: %d/%d"
#define CHAT_INFO_FORMAT "%s\n" CHAR_RESOURCES_FORMAT

#define HORIZONTAL_LINE "--------------------------------------------------"
#define COMBAT_HEAD_FORMAT CHAT_INFO_FORMAT "\n" HORIZONTAL_LINE "\n" CHAT_INFO_FORMAT

enum combat_mode_index {
    //strings that are updated via local
    LEVEL_STR,
    HEALTH_STR,
    STAMINA_STR,
    MANA_STR,
    COMBAT_NEXT_ACTION,
    USE_ABILITY,
    USE_POTION,
    USE_WHICH_ABILITY,
    USE_WHICH_POTION,
    SUBMENU_RETURN_TEXT,
    //strings that are prepared in advance
    ENEMY_NAME_LVL,
    PLAYER_NAME_LVL,
    //strings that are updated after an action
    COMBAT_HEAD,
    MAX_COMBAT_MODE_STRINGS
};

char** combat_mode_strings = NULL;

menu_t combat_mode_main_menu;
menu_t combat_mode_ability_menu;
menu_t combat_mode_potion_menu;

void update_combat_mode_local(void);

void update_combat_head(const character_t* player, const character_t* enemy);

int init_combat_mode() {
    combat_mode_strings = (char**) malloc(sizeof(char*) * MAX_COMBAT_MODE_STRINGS);
    RETURN_WHEN_NULL(combat_mode_strings, 1, "Combat Mode", "Failed to allocate memory for combat mode strings.");

    for (int i = 0; i < MAX_COMBAT_MODE_STRINGS; i++) {
        combat_mode_strings[i] = NULL;
    }

    combat_mode_main_menu.options = &combat_mode_strings[USE_ABILITY];
    combat_mode_main_menu.option_count = MAX_COMBAT_MAIN_OPTIONS;
    combat_mode_main_menu.selected_index = 0;
    combat_mode_main_menu.tailing_text = " ";

    update_combat_mode_local();
    observe_local(update_combat_mode_local);
    return 0;
}

state_t prepare_combat_mode(const character_t* player, const character_t* enemy) {
    RETURN_WHEN_NULL(combat_mode_strings, EXIT_GAME, "Combat Mode", "Combat mode is not initialized.")
    RETURN_WHEN_NULL(player, EXIT_GAME, "Combat Mode", "Player is NULL.")
    RETURN_WHEN_NULL(enemy, EXIT_GAME, "Combat Mode", "Enemy is NULL.")
    // free the previously prepared strings
    for (int i = ENEMY_NAME_LVL; i < COMBAT_HEAD; i++) {
        if (combat_mode_strings[i] != NULL) {
            free(combat_mode_strings[i]);
        }
    }

    // prepare enemy name and level string
    char* enemy_name = get_local_string(enemy->name);
    char* enemy_name_lvl_str = malloc(64);
    snprintf(enemy_name_lvl_str, 64, CHAR_NAME_LVL_FORMAT,
             enemy_name, combat_mode_strings[LEVEL_STR], enemy->level);
    combat_mode_strings[ENEMY_NAME_LVL] = enemy_name_lvl_str;
    free(enemy_name);

    // prepare player name and level string
    char* player_name_lvl_str = malloc(64);
    snprintf(player_name_lvl_str, 64, CHAR_NAME_LVL_FORMAT,
             player->name, combat_mode_strings[LEVEL_STR], player->level);
    combat_mode_strings[PLAYER_NAME_LVL] = player_name_lvl_str;

    update_combat_head(player, enemy);
    return COMBAT_MODE;
}

state_t update_combat_mode(input_t input, character_t* player, character_t* enemy) {
    RETURN_WHEN_NULL(combat_mode_strings, EXIT_GAME, "Combat Mode", "Combat mode is not initialized.")
    RETURN_WHEN_NULL(player, EXIT_GAME, "Combat Mode", "Player is NULL.")
    RETURN_WHEN_NULL(enemy, EXIT_GAME, "Combat Mode", "Enemy is NULL.")

    print_text(5, 2, TB_WHITE, TB_DEFAULT, combat_mode_strings[COMBAT_HEAD]);


    return COMBAT_MODE;
}

void free_prepared_resources() {
    for (int i = ENEMY_NAME_LVL; i < MAX_COMBAT_MODE_STRINGS; i++) {
        if (combat_mode_strings[i] != NULL) {
            free(combat_mode_strings[i]);
            combat_mode_strings[i] = NULL;
        }
    }
}

void shutdown_combat_mode() {
    if (combat_mode_strings != NULL) {
        for (int i = 0; i < MAX_COMBAT_MODE_STRINGS; i++) {
            if (combat_mode_strings[i] != NULL) free(combat_mode_strings[i]);
        }
        free(combat_mode_strings);
    }
}
void update_combat_mode_local(void) {
    // free the previous local strings
    for (int i = 0; i < ENEMY_NAME_LVL; i++) {
        if (combat_mode_strings[i] != NULL) {
            free(combat_mode_strings[i]);
        }
    }

    combat_mode_strings[LEVEL_STR] = get_local_string("LEVEL");
    combat_mode_strings[HEALTH_STR] = get_local_string("HEALTH");
    combat_mode_strings[STAMINA_STR] = get_local_string("STAMINA");
    combat_mode_strings[MANA_STR] = get_local_string("MANA");

    combat_mode_strings[COMBAT_NEXT_ACTION] = get_local_string("COMBAT.NEXT_ACTION");
    combat_mode_main_menu.title = combat_mode_strings[COMBAT_NEXT_ACTION];
    combat_mode_strings[USE_ABILITY] = get_local_string("COMBAT.USE_ABILITY");
    combat_mode_strings[USE_POTION] = get_local_string("COMBAT.USE_POTION");

    combat_mode_strings[USE_WHICH_ABILITY] = get_local_string("COMBAT.ABILITY.WHICH");
    combat_mode_ability_menu.title = combat_mode_strings[USE_WHICH_ABILITY];
    combat_mode_strings[USE_WHICH_POTION] = get_local_string("COMBAT.POTION.WHICH");
    combat_mode_potion_menu.title = combat_mode_strings[USE_WHICH_POTION];
    combat_mode_strings[SUBMENU_RETURN_TEXT] = get_local_string("PRESS_ESC.RETURN");
    combat_mode_ability_menu.tailing_text = combat_mode_strings[SUBMENU_RETURN_TEXT];
    combat_mode_potion_menu.tailing_text = combat_mode_strings[SUBMENU_RETURN_TEXT];
}

void update_combat_head(const character_t* player, const character_t* enemy) {
    // free the previous head string
    if (combat_mode_strings[COMBAT_HEAD] != NULL) {
        free(combat_mode_strings[COMBAT_HEAD]);
    }

    // update the combat head string
    char* combat_head_str = malloc(256);
    snprintf(combat_head_str, 256, COMBAT_HEAD_FORMAT,
             combat_mode_strings[ENEMY_NAME_LVL],
             combat_mode_strings[HEALTH_STR], enemy->current_resources.health, enemy->max_resources.health,
             combat_mode_strings[STAMINA_STR], enemy->current_resources.stamina, enemy->max_resources.stamina,
             combat_mode_strings[MANA_STR], enemy->current_resources.mana, enemy->max_resources.mana,
             combat_mode_strings[PLAYER_NAME_LVL],
             combat_mode_strings[HEALTH_STR], player->current_resources.health, player->max_resources.health,
             combat_mode_strings[STAMINA_STR], player->current_resources.stamina, player->max_resources.stamina,
             combat_mode_strings[MANA_STR], player->current_resources.mana, player->max_resources.mana);
    combat_mode_strings[COMBAT_HEAD] = combat_head_str;
}
