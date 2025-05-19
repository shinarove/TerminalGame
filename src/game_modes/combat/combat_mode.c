#include "combat_mode.h"

#include "../../game_mechanics/ability_usage.h"
#include "../../game_mechanics/leveling.h"
#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../io/output/common/common_output.h"
#include "../../logger/logger.h"
#include "../../io/output/specific/character_output.h"

#include <stdlib.h>

#define COMBAT_Y_POS_PLAYER_INFO 2
#define COMBAT_Y_POS_ENEMY_INFO 6
#define COMBAT_Y_POS_BODY 11
#define COMBAT_Y_POS_FOOTER 13

#define MAX_COMBAT_MAIN_OPTIONS 2

// [Enemy Name] "used" [Ability Name], [Result]
#define ENEMY_ABILITY_USAGE_FORMAT "%s %s %s, %s"
// [Part 1] [Enemy Name] [Part 2]
#define VICTORY_MESSAGE_FORMAT "%s %s %s"
// [Msg] [Enemy Name]
#define DEATH_MESSAGE_FORMAT "%s %s"

typedef enum {
    CHOOSE_ABILITY_POTION,
    ABILITY_SELECTION,
    CHECK_POTION_INV,
    POTION_SELECTION,
    NOT_ENOUGH_RESOURCES,
    WAIT_AFTER_PLAYER_ACTION,
    ENEMY_TURN,
    WAIT_AFTER_ENEMY_ACTION,
    EXIT_COMBAT_MODE// enemy or player died
} combat_mode_state_t;

enum combat_mode_index {
    //strings that are updated via local
    USED_STR,   // String for 'used'
    COMBAT_NEXT_ACTION,
    USE_ABILITY,
    USE_POTION,
    USE_WHICH_ABILITY,
    USE_WHICH_POTION,
    SUBMENU_RETURN_TEXT,
    EMPTY_POTION_BAG,
    CONTINUE_TEXT,
    VICTORY_TEXT_PART1,
    VICTORY_TEXT_PART2,
    DEATH_TEXT,
    ENEMY_ABILITY_HIT,
    ENEMY_ABILITY_MISSED,
    ENEMY_ABILITY_FAILED,
    NOT_ENOUGH_HEALTH_INFO,
    NOT_ENOUGH_STAMINA_INFO,
    NOT_ENOUGH_MANA_INFO,
    PC_ABILITY_USAGE_SUCCESS,
    PC_ABILITY_USAGE_FAILED,
    PC_ABILITY_USAGE_MISSED,
    //strings that are prepared in advance
    ENEMY_NAME,
    //strings that are updated after an action
    NOT_ENOUGH_RES_INFO,
    PLAYER_ABILITY_USAGE_INFO,
    ENEMY_ABILITY_USAGE_INFO,
    COMBAT_END_MSG,
    MAX_COMBAT_MODE_STRINGS
};

char** combat_mode_strings = NULL;

menu_t combat_mode_main_menu;
menu_t combat_mode_ability_menu;
menu_t combat_mode_potion_menu;

combat_mode_state_t combat_state = CHOOSE_ABILITY_POTION;
state_t exit_combat_mode_with = MAP_MODE;

void update_combat_mode_local(void);

state_t evaluate_player_ability_usage(usage_result_t result, character_t* player, const character_t* enemy);

state_t evaluate_enemy_ability_usage(usage_result_t result, const character_t* player, const character_t* enemy);

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
    combat_mode_main_menu.args = NULL;

    combat_mode_ability_menu.options = NULL;
    combat_mode_ability_menu.args = NULL;
    combat_mode_potion_menu.options = NULL;
    combat_mode_potion_menu.args = NULL;

    update_combat_mode_local();
    observe_local(update_combat_mode_local);
    return 0;
}

state_t prepare_combat_mode(const character_t* player, const character_t* enemy) {
    RETURN_WHEN_NULL(combat_mode_strings, EXIT_GAME, "Combat Mode", "Combat mode is not initialized.")
    RETURN_WHEN_NULL(player, EXIT_GAME, "Combat Mode", "Player is NULL.")
    RETURN_WHEN_NULL(enemy, EXIT_GAME, "Combat Mode", "Enemy is NULL.")
    // free the previously prepared strings
    for (int i = ENEMY_NAME; i < NOT_ENOUGH_RES_INFO; i++) {
        if (combat_mode_strings[i] != NULL) {
            free(combat_mode_strings[i]);
        }
    }
    // reset the selected index in the menus
    combat_mode_main_menu.selected_index = 0;
    combat_mode_ability_menu.selected_index = 0;
    combat_mode_potion_menu.selected_index = 0;

    combat_state = CHOOSE_ABILITY_POTION;

    // prepare the enemy name string
    combat_mode_strings[ENEMY_NAME] = get_local_string(enemy->name);

    // prepare the ability menu
    if (combat_mode_ability_menu.options != NULL) free(combat_mode_ability_menu.options);
    RETURN_WHEN_TRUE(player->abilities->ability_count == 0, EXIT_GAME, "Combat Mode", "In `prepare_combat_mode` player has no abilities.")
    char** ability_names = malloc(sizeof(char*) * player->abilities->ability_count);
    combat_mode_ability_menu.options = ability_names;
    for (int i = 0; i < player->abilities->ability_count; i++) {
        ability_names[i] = player->abilities->abilities[i]->local_name;
    }
    combat_mode_ability_menu.option_count = player->abilities->ability_count;
    combat_mode_ability_menu.selected_index = 0;

    // prepare the potion menu
    if (combat_mode_potion_menu.options != NULL) free(combat_mode_potion_menu.options);
    combat_mode_potion_menu.options = NULL;
    combat_mode_potion_menu.selected_index = 0;
    combat_mode_potion_menu.option_count = 0;

    // update_combat_head(player, enemy);

    // print the player & enemy info once with update
    print_c_hori(5, COMBAT_Y_POS_PLAYER_INFO, player,
                 (output_args_c_t) {true, true, true});
    print_c_hori(5, COMBAT_Y_POS_ENEMY_INFO, player,
                 (output_args_c_t) {true, true, true});
    return COMBAT_MODE;
}

state_t update_combat_mode(const input_t input, character_t* player, character_t* enemy) {
    RETURN_WHEN_NULL(combat_mode_strings, EXIT_GAME, "Combat Mode", "Combat mode is not initialized.")
    RETURN_WHEN_NULL(player, EXIT_GAME, "Combat Mode", "Player is NULL.")
    RETURN_WHEN_NULL(enemy, EXIT_GAME, "Combat Mode", "Enemy is NULL.")

    const output_args_c_t args_no_update = {false, true, true};
    const output_args_c_t args_update = {true, true, true};

    print_c_hori(5, COMBAT_Y_POS_PLAYER_INFO, player, args_no_update);
    print_c_hori(5, COMBAT_Y_POS_ENEMY_INFO, enemy, args_no_update);

    state_t res = COMBAT_MODE;
    switch (combat_state) {
        case CHOOSE_ABILITY_POTION:
            switch (handle_simple_menu(input, 5, COMBAT_Y_POS_BODY, &combat_mode_main_menu)) {
                case 0:
                    combat_state = ABILITY_SELECTION;
                    clear_screen();
                    break;
                case 1:
                    combat_state = CHECK_POTION_INV;
                    clear_screen();
                    break;
                case MAX_COMBAT_MAIN_OPTIONS:// nothing was done, do nothing
                case -1:
                    break;// ESC was pressed, do nothing
                case -2:
                    res = EXIT_GAME;// Ctrl + C was pressed
                    break;
                default:
                    log_msg(WARNING, "Combat Mode", "Invalid option returned in handle_menu: %d", res);
                    break;
            }
            break;
        case ABILITY_SELECTION:
            const int selected_index = handle_simple_menu(input, 5, COMBAT_Y_POS_BODY, &combat_mode_ability_menu);
            switch (selected_index) {
                case -1:// ESC was pressed
                    combat_state = CHOOSE_ABILITY_POTION;
                    combat_mode_ability_menu.selected_index = 0;
                    clear_screen();
                    break;
                case -2:
                    res = EXIT_GAME;// Ctrl + C was pressed
                    break;
                default:
                    if (selected_index >= 0 && selected_index < combat_mode_ability_menu.option_count) {
                        // a valid ability was selected
                        const ability_t* ability = get_ability_by_index_c(player, selected_index);
                        RETURN_WHEN_NULL(ability, EXIT_GAME, "Combat Mode",
                                         "In `update_combat_mode` a valid ability index %d was selected, but the returned ability is NULL", selected_index)
                        const usage_result_t result = use_ability(player, enemy, ability);
                        res = evaluate_player_ability_usage(result, player, enemy);
                        if (result == SUCCESS || result == TARGET_DIED) {
                            print_c_hori(5, COMBAT_Y_POS_PLAYER_INFO, player, args_update);
                            print_c_hori(5, COMBAT_Y_POS_ENEMY_INFO, enemy, args_update);
                        }
                    } else if (selected_index != combat_mode_ability_menu.option_count) {
                        log_msg(WARNING, "Combat Mode", "Invalid option returned in handle_menu: %d", selected_index);
                    }
                    break;
            }
            break;
        case CHECK_POTION_INV:
            if (combat_mode_potion_menu.option_count == 0) {
                print_text(5, COMBAT_Y_POS_BODY, WHITE, DEFAULT, combat_mode_strings[EMPTY_POTION_BAG]);
                print_text(5, COMBAT_Y_POS_FOOTER, WHITE, DEFAULT, combat_mode_strings[SUBMENU_RETURN_TEXT]);
                if (input == ESCAPE) {
                    combat_state = CHOOSE_ABILITY_POTION;
                    clear_screen();
                }
            } else {
                combat_state = POTION_SELECTION;
                clear_screen();
            }
            break;
        case POTION_SELECTION:
            log_msg(DEBUG, "Combat Mode", "POTION_SELECTION");
            break;
        case WAIT_AFTER_PLAYER_ACTION:
            print_text(5, COMBAT_Y_POS_BODY, WHITE, DEFAULT, combat_mode_strings[PLAYER_ABILITY_USAGE_INFO]);
            print_text(5, COMBAT_Y_POS_FOOTER, WHITE, DEFAULT, combat_mode_strings[CONTINUE_TEXT]);
            if (input == ENTER) {
                combat_state = ENEMY_TURN;
                clear_screen();
            }
            break;
        case ENEMY_TURN:
            // TODO: randomise usage of ability
            const ability_t* ability = get_ability_by_index_c(enemy, 0);
            RETURN_WHEN_NULL(ability, EXIT_GAME, "Combat Mode", "In `update_combat_mode` enemy has no abilities.")
            const usage_result_t result = use_ability(enemy, player, ability);
            res = evaluate_enemy_ability_usage(result, player, enemy);
            if (result == SUCCESS || result == TARGET_DIED) {
                print_c_hori(5, COMBAT_Y_POS_PLAYER_INFO, player, args_update);
                print_c_hori(5, COMBAT_Y_POS_ENEMY_INFO, enemy, args_update);
            }
            break;
        case WAIT_AFTER_ENEMY_ACTION:
            print_text(5, COMBAT_Y_POS_BODY, WHITE, DEFAULT, combat_mode_strings[ENEMY_ABILITY_USAGE_INFO]);
            print_text(5, COMBAT_Y_POS_FOOTER, WHITE, DEFAULT, combat_mode_strings[CONTINUE_TEXT]);
            if (input == ENTER) {
                combat_state = CHOOSE_ABILITY_POTION;
                clear_screen();
            }
            break;
        case NOT_ENOUGH_RESOURCES:
            print_text(5, COMBAT_Y_POS_BODY, WHITE, DEFAULT, combat_mode_strings[NOT_ENOUGH_RES_INFO]);
            print_text(5, COMBAT_Y_POS_FOOTER, WHITE, DEFAULT, combat_mode_strings[SUBMENU_RETURN_TEXT]);
            if (input == ESCAPE) {
                combat_state = CHOOSE_ABILITY_POTION;
                clear_screen();
            }
            break;
        case EXIT_COMBAT_MODE:
            print_text(5, COMBAT_Y_POS_BODY, WHITE, DEFAULT, combat_mode_strings[COMBAT_END_MSG]);
            print_text(5, COMBAT_Y_POS_FOOTER, WHITE, DEFAULT, combat_mode_strings[CONTINUE_TEXT]);
            if (input == ENTER) {
                res = exit_combat_mode_with;
                clear_screen();
            }
            break;
    }

    return res;
}

void shutdown_combat_mode() {
    if (combat_mode_strings != NULL) {
        for (int i = 0; i < MAX_COMBAT_MODE_STRINGS; i++) {
            if (i == NOT_ENOUGH_RES_INFO) continue; // this string always has a copy of a local string
            if (i == PLAYER_ABILITY_USAGE_INFO) continue; // this string always has a copy of a local string
            if (combat_mode_strings[i] != NULL) free(combat_mode_strings[i]);
        }
        free(combat_mode_strings);
    }
    if (combat_mode_ability_menu.options != NULL) free(combat_mode_ability_menu.options);
    if (combat_mode_potion_menu.options != NULL) free(combat_mode_potion_menu.options);
}

void update_combat_mode_local(void) {
    // free the previous local strings
    for (int i = 0; i < ENEMY_NAME; i++) {
        if (combat_mode_strings[i] != NULL) {
            free(combat_mode_strings[i]);
        }
    }

    combat_mode_strings[USED_STR] = get_local_string("USED.LOWERCASE");

    combat_mode_strings[COMBAT_NEXT_ACTION] = get_local_string("COMBAT.NEXT_ACTION");
    combat_mode_main_menu.title = combat_mode_strings[COMBAT_NEXT_ACTION];
    combat_mode_strings[USE_ABILITY] = get_local_string("COMBAT.USE_ABILITY");
    combat_mode_strings[USE_POTION] = get_local_string("COMBAT.USE_POTION");

    combat_mode_strings[USE_WHICH_ABILITY] = get_local_string("COMBAT.ABILITY.WHICH");
    combat_mode_ability_menu.title = combat_mode_strings[USE_WHICH_ABILITY];
    combat_mode_strings[USE_WHICH_POTION] = get_local_string("COMBAT.POTION.WHICH");
    combat_mode_potion_menu.title = combat_mode_strings[USE_WHICH_POTION];

    combat_mode_strings[EMPTY_POTION_BAG] = get_local_string("COMBAT.POTION.EMPTY");

    combat_mode_strings[SUBMENU_RETURN_TEXT] = get_local_string("PRESS_ESC.RETURN");
    combat_mode_ability_menu.tailing_text = combat_mode_strings[SUBMENU_RETURN_TEXT];
    combat_mode_potion_menu.tailing_text = combat_mode_strings[SUBMENU_RETURN_TEXT];

    combat_mode_strings[CONTINUE_TEXT] = get_local_string("PRESS_ENTER.CONTINUE");
    combat_mode_strings[VICTORY_TEXT_PART1] = get_local_string("COMBAT.PLAYER.VICTORY1");
    combat_mode_strings[VICTORY_TEXT_PART2] = get_local_string("COMBAT.PLAYER.VICTORY2");
    combat_mode_strings[DEATH_TEXT] = get_local_string("COMBAT.PLAYER.DEATH_MSG");

    combat_mode_strings[ENEMY_ABILITY_HIT] = get_local_string("COMBAT.ENEMY.ABILITY.HIT");
    combat_mode_strings[ENEMY_ABILITY_MISSED] = get_local_string("COMBAT.ENEMY.ABILITY.MISS");
    combat_mode_strings[ENEMY_ABILITY_FAILED] = get_local_string("COMBAT.ENEMY.ABILITY.FAIL");

    combat_mode_strings[NOT_ENOUGH_HEALTH_INFO] = get_local_string("COMBAT.RESOURCE.NOT_ENOUGH.HEALTH");
    combat_mode_strings[NOT_ENOUGH_STAMINA_INFO] = get_local_string("COMBAT.RESOURCE.NOT_ENOUGH.STAMINA");
    combat_mode_strings[NOT_ENOUGH_MANA_INFO] = get_local_string("COMBAT.RESOURCE.NOT_ENOUGH.MANA");

    combat_mode_strings[PC_ABILITY_USAGE_SUCCESS] = get_local_string("COMBAT.PLAYER.ABILITY.HIT");
    combat_mode_strings[PC_ABILITY_USAGE_FAILED] = get_local_string("COMBAT.PLAYER.ABILITY.FAIL");
    combat_mode_strings[PC_ABILITY_USAGE_MISSED] = get_local_string("COMBAT.PLAYER.ABILITY.MISS");
}

// TODO: add a parameter for the used ability
state_t evaluate_player_ability_usage(const usage_result_t result, character_t* player, const character_t* enemy) {
    state_t res = COMBAT_MODE;
    switch (result) {
        case SUCCESS:
            combat_state = WAIT_AFTER_PLAYER_ACTION;
            combat_mode_strings[PLAYER_ABILITY_USAGE_INFO] = combat_mode_strings[PC_ABILITY_USAGE_SUCCESS];
            clear_screen();
            break;
        case NOT_ENOUGH_HEALTH:
            combat_state = NOT_ENOUGH_RESOURCES;
            combat_mode_strings[NOT_ENOUGH_RES_INFO] = combat_mode_strings[NOT_ENOUGH_HEALTH_INFO];
            clear_screen();
            break;
        case NOT_ENOUGH_STAMINA:
            combat_state = NOT_ENOUGH_RESOURCES;
            combat_mode_strings[NOT_ENOUGH_RES_INFO] = combat_mode_strings[NOT_ENOUGH_STAMINA_INFO];
            clear_screen();
            break;
        case NOT_ENOUGH_MANA:
            combat_state = NOT_ENOUGH_RESOURCES;
            combat_mode_strings[NOT_ENOUGH_RES_INFO] = combat_mode_strings[NOT_ENOUGH_MANA_INFO];
            clear_screen();
            break;
        case MISSED:
            combat_state = WAIT_AFTER_PLAYER_ACTION;
            combat_mode_strings[PLAYER_ABILITY_USAGE_INFO] = combat_mode_strings[PC_ABILITY_USAGE_MISSED];
            clear_screen();
            break;
        case FAILED:
            combat_state = WAIT_AFTER_PLAYER_ACTION;
            combat_mode_strings[PLAYER_ABILITY_USAGE_INFO] = combat_mode_strings[PC_ABILITY_USAGE_FAILED];
            clear_screen();
            break;
        case TARGET_DIED:
            combat_state = EXIT_COMBAT_MODE;
            exit_combat_mode_with = MAP_MODE;
            if (combat_mode_strings[COMBAT_END_MSG] != NULL) free(combat_mode_strings[COMBAT_END_MSG]);
            char* buffer = malloc(64);
            snprintf(buffer, 64, VICTORY_MESSAGE_FORMAT,
                     combat_mode_strings[VICTORY_TEXT_PART1], combat_mode_strings[ENEMY_NAME], combat_mode_strings[VICTORY_TEXT_PART2]);
            combat_mode_strings[COMBAT_END_MSG] = buffer;

            gain_exp(player, enemy);

            clear_screen();
            break;
        case UNEXPECTED_ERROR:
            res = EXIT_GAME;// unexpected error
            break;
    }
    return res;
}

// TODO: add a parameter for the used ability
state_t evaluate_enemy_ability_usage(const usage_result_t result, const character_t* player, const character_t* enemy) {
    state_t res = COMBAT_MODE;

    char* char_buffer = NULL;
    switch (result) {
        case SUCCESS:
            combat_state = WAIT_AFTER_ENEMY_ACTION;
            if (combat_mode_strings[ENEMY_ABILITY_USAGE_INFO] != NULL) free(combat_mode_strings[ENEMY_ABILITY_USAGE_INFO]);
            char_buffer = malloc(64);
            snprintf(char_buffer, 64, ENEMY_ABILITY_USAGE_FORMAT,
                     combat_mode_strings[ENEMY_NAME], combat_mode_strings[USED_STR], enemy->abilities->abilities[0]->local_name,//TODO print used ability
                     combat_mode_strings[ENEMY_ABILITY_HIT]);
            combat_mode_strings[ENEMY_ABILITY_USAGE_INFO] = char_buffer;
            clear_screen();
            break;
        case NOT_ENOUGH_HEALTH:
        case NOT_ENOUGH_STAMINA:
        case NOT_ENOUGH_MANA:
            // this should not happen!
            log_msg(ERROR, "Combat Mode",
                    "In `evaluate_enemy_ability_usage` the enemy tried to use an ability, but hasn't enough resources.");
            res = EXIT_GAME;
            break;
        case MISSED:
            combat_state = WAIT_AFTER_ENEMY_ACTION;
            if (combat_mode_strings[ENEMY_ABILITY_USAGE_INFO] != NULL) free(combat_mode_strings[ENEMY_ABILITY_USAGE_INFO]);
            char_buffer = malloc(64);
            snprintf(char_buffer, 64, ENEMY_ABILITY_USAGE_FORMAT,
                     combat_mode_strings[ENEMY_NAME], combat_mode_strings[USED_STR], enemy->abilities->abilities[0]->local_name,//TODO print used ability
                     combat_mode_strings[ENEMY_ABILITY_MISSED]);
            combat_mode_strings[ENEMY_ABILITY_USAGE_INFO] = char_buffer;
            clear_screen();
            break;
        case FAILED:
            combat_state = WAIT_AFTER_ENEMY_ACTION;
            if (combat_mode_strings[ENEMY_ABILITY_USAGE_INFO] != NULL) free(combat_mode_strings[ENEMY_ABILITY_USAGE_INFO]);
            char_buffer = malloc(64);
            snprintf(char_buffer, 64, ENEMY_ABILITY_USAGE_FORMAT,
                     combat_mode_strings[ENEMY_NAME], combat_mode_strings[USED_STR], enemy->abilities->abilities[0]->local_name,//TODO print used ability
                     combat_mode_strings[ENEMY_ABILITY_FAILED]);
            combat_mode_strings[ENEMY_ABILITY_USAGE_INFO] = char_buffer;
            clear_screen();
            break;
        case TARGET_DIED:
            // player died
            combat_state = EXIT_COMBAT_MODE;
            exit_combat_mode_with = GAME_OVER;
            if (combat_mode_strings[COMBAT_END_MSG] != NULL) free(combat_mode_strings[COMBAT_END_MSG]);
            char_buffer = malloc(64);
            snprintf(char_buffer, 64, DEATH_MESSAGE_FORMAT,
                     combat_mode_strings[DEATH_TEXT], combat_mode_strings[ENEMY_NAME]);
            combat_mode_strings[COMBAT_END_MSG] = char_buffer;
            clear_screen();
            break;
        case UNEXPECTED_ERROR:
            res = EXIT_GAME;
            break;
    }

    return res;
}
