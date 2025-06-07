#include "character_mode.h"

#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../io/output/specific/character_output.h"
#include "../../logger/logger.h"

#include <stdlib.h>

#define MAX_OPTIONS_INV_MANIP 3
#define MAX_OPTIONS_EQUIP_MANIP 2

#define CM_Y_POS_TITLE 2
#define CM_Y_POS_CT_ABIL_OV (CM_Y_POS_TITLE + 1)
#define CM_Y_POS_PLAYER_HEAD (CM_Y_POS_CT_ABIL_OV + 2)
#define CM_Y_POS_BODY (CM_Y_POS_PLAYER_HEAD + 5)

#define CM_X_POS_COLUMN1 5
#define CM_X_POS_COLUMN2 (CM_X_POS_COLUMN1 + 25)
#define CM_X_POS_COLUMN3 (CM_X_POS_COLUMN2 + 25)

typedef enum {
    INVENTORY_MENU,
    INV_MANIPULATION,
    EQUIPMENT_MENU,
    EQUIP_MANIPULATION,
    ABILITY_MENU,
    EXIT_CHARACTER_MODE,
} char_mode_state_t;

enum cm_index {
    CHAR_MODE_TITLE,
    CHANGE_TO_ABILITY_OV,
    CHANGE_TO_INVENTORY_OV,

    INVENTORY_TITLE,
    EQUIPMENT_TITLE,
    MANIP_TITLE,
    PRESS_ESC_CANCEL,

    EMPTY_GEAR_SLOT,
    HEAD_STR,
    BODY_STR,
    LEGS_STR,
    HANDS_STR,
    RING_LEFT_STR,
    RING_RIGHT_STR,
    AMULET_STR,
    MAIN_HAND_STR,
    OFF_HAND_STR,

    EQUIP_GEAR_STR,
    DROP_GEAR_STR,
    SHOW_GEAR_DETAILS_STR,
    UNEQUIP_GEAR_STR,

    MAX_CHAR_MODE_STRINGS
};

char** cm_strings = NULL;

menu_t cm_inv_menu;
menu_arg_t cm_inv_menu_arg;

menu_t cm_equip_menu;
menu_arg_t cm_equip_menu_arg;

menu_t cm_ability_menu;

// manipulation menu for the inventory
menu_t cm_inv_manip_menu;
// manipulation menu for the equipped gear
menu_t cm_equip_manip_menu;

char_mode_state_t cm_state = INVENTORY_MENU;

int update_inventory_menu(input_t input, int menu_result);

void update_character_mode_local(void);

int init_character_mode() {
    cm_strings = (char**) malloc(sizeof(char*) * MAX_CHAR_MODE_STRINGS);
    RETURN_WHEN_NULL(cm_strings, 1, "Character Mode", "Failed to allocate memory for character mode strings.")

    for (int i = 0; i < MAX_CHAR_MODE_STRINGS; i++) {
        cm_strings[i] = NULL;
    }

    cm_inv_menu = (menu_t) {NULL, NULL, 0, 0, " ", NULL};
    cm_inv_menu_arg = (menu_arg_t) {ACTIVE, BLACK, WHITE, WHITE, DEFAULT};

    cm_equip_menu = (menu_t) {NULL, NULL, MAX_GEAR_SLOTS, 0, " ", NULL};
    cm_equip_menu_arg = (menu_arg_t) {INACTIVE_WOUT_SEL, BLACK, WHITE, WHITE, DEFAULT};

    cm_ability_menu = (menu_t) {NULL, NULL, 0, 0, " ", NULL};
    cm_inv_manip_menu = (menu_t) {
            NULL,
            &cm_strings[EQUIP_GEAR_STR],
            MAX_OPTIONS_INV_MANIP,
            0,
            NULL,
            NULL};
    cm_equip_manip_menu = (menu_t) {
            NULL,
            &cm_strings[SHOW_GEAR_DETAILS_STR],
            MAX_OPTIONS_EQUIP_MANIP,
            0,
            NULL,
            NULL};

    // connecting the args to the menus
    cm_inv_menu.args = &cm_inv_menu_arg;
    cm_equip_menu.args = &cm_equip_menu_arg;

    // allocate memory for the equipment options
    cm_equip_menu.options = (char**) malloc(sizeof(char*) * MAX_GEAR_SLOTS);// only until the both hand slot
    RETURN_WHEN_NULL(cm_equip_menu.options, 1, "Character Mode", "Failed to allocate memory for equipment options.")
    for (int i = 0; i < MAX_GEAR_SLOTS; i++) {
        cm_equip_menu.options[i] = NULL;
    }

    update_character_mode_local();
    observe_local(update_character_mode_local);
    return 0;
}

state_t prepare_character_mode(const Character* player) {
    RETURN_WHEN_NULL(player, EXIT_GAME, "Character Mode", "In `prepare_character_mode` given player is NULL.")
    cm_state = INVENTORY_MENU;// reset the state

    // reset the menus
    cm_inv_menu.selected_index = 0;
    cm_inv_menu.args->mode = ACTIVE;

    cm_equip_menu.selected_index = 0;
    cm_equip_menu.args->mode = INACTIVE_WOUT_SEL;

    cm_ability_menu.selected_index = 0;
    cm_ability_menu.args->mode = INACTIVE_WOUT_SEL;

    cm_ability_menu.selected_index = 0;

    // prepare the inventory menu
    if (cm_inv_menu.options != NULL) {
        for (int i = 0; i < cm_inv_menu.option_count; i++) {
            if (cm_inv_menu.options[i] != NULL) {
                free(cm_inv_menu.options[i]);
                cm_inv_menu.options[i] = NULL;// reset the option
            }
        }
        free(cm_inv_menu.options);
        cm_inv_menu.options = NULL;// reset the options pointer
    }
    char* buffer[64];
    cm_inv_menu.options = malloc(sizeof(char*) * player->inventory->gear_list->size);
    if (player->inventory != NULL) {
        cm_inv_menu.option_count = player->inventory->gear_list->size;
        for (int i = 0; i < player->inventory->gear_list->size; i++) {
            const gear_t* gear = player->vtable->get_gear_at(player, i);
            const int is_equipped = player->vtable->is_gear_equipped(player, gear);
            if (is_equipped == 1) {
                // mark the gear as equipped
                snprintf(buffer, sizeof(buffer), "*%s*", gear->local_name);
            } else if (is_equipped == 0) {
                // gear is not equipped
                snprintf(buffer, sizeof(buffer), "%s", gear->local_name);
            } else {
                // gear is not rightfully equipped or an error occurred
            }
            cm_inv_menu.options[i] = strdup(buffer);
        }
    } else {
        cm_inv_menu.option_count = 0;
    }

    // prepare the equipment menu
    for (int i = 0; i < MAX_GEAR_SLOTS; i++) {
        if (cm_equip_menu.options[i] != NULL) {
            free(cm_equip_menu.options[i]);
        }
    }
    for (int i = 0; i < MAX_GEAR_SLOTS; i++) {
        if (player->inventory != NULL && player->inventory->equipped[i] != NULL) {
            snprintf(buffer, sizeof(buffer), "%s: %s",
                     cm_strings[HEAD_STR + i], player->inventory->equipped[i]->local_name);
        } else {
            snprintf(buffer, sizeof(buffer), "%s: %s",
                     cm_strings[HEAD_STR + i], cm_strings[EMPTY_GEAR_SLOT]);
        }
        cm_inv_menu.options[i] = strdup(buffer);
    }

    return CHARACTER_MODE;
}

state_t update_character_mode(const input_t input, Character* player) {
    RETURN_WHEN_NULL(player, EXIT_GAME, "Character Mode", "In `update_character_mode` given player is NULL.")

    state_t res = CHARACTER_MODE;

    print_text(CM_X_POS_COLUMN1, CM_Y_POS_TITLE, WHITE, DEFAULT, cm_strings[CHAR_MODE_TITLE]);
    const output_args_c_t lvl_up_args = {0, RES_CURR_MAX, ATTR_MAX_BONUS};
    print_char_h(CM_X_POS_COLUMN1, CM_Y_POS_PLAYER_HEAD, player, lvl_up_args);

    int inv_menu_res = cm_inv_menu.option_count;
    int inv_manip_menu_res = MAX_OPTIONS_INV_MANIP;
    int equip_menu_res = cm_equip_menu.option_count;
    int equip_manip_menu_res = cm_equip_manip_menu.option_count;
    int abil_menu_res = cm_ability_menu.option_count;

    // handle the prints
    if (cm_state == ABILITY_MENU) {
        print_text_f(CM_X_POS_COLUMN1, CM_Y_POS_CT_ABIL_OV, WHITE, DEFAULT, "%s     ",
                     cm_strings[CHANGE_TO_INVENTORY_OV]);

        abil_menu_res = handle_simple_menu(input, CM_X_POS_COLUMN1, CM_Y_POS_BODY, &cm_ability_menu);
    } else {
        print_text_f(CM_X_POS_COLUMN1, CM_Y_POS_CT_ABIL_OV, WHITE, DEFAULT, "%s     ",
                     cm_strings[CHANGE_TO_ABILITY_OV]);
        inv_menu_res = handle_simple_menu(input, CM_X_POS_COLUMN1, CM_Y_POS_BODY, &cm_inv_menu);
        equip_menu_res = handle_simple_menu(input, CM_X_POS_COLUMN2, CM_Y_POS_BODY, &cm_equip_menu);

        // additional menus for the inventory and equipment manipulation
        if (cm_state == INV_MANIPULATION) {
            inv_manip_menu_res = handle_simple_menu(input, CM_X_POS_COLUMN3, CM_Y_POS_BODY, &cm_inv_manip_menu);
        } else if (cm_state == EQUIP_MANIPULATION) {
            equip_manip_menu_res = handle_simple_menu(input, CM_X_POS_COLUMN3, CM_Y_POS_BODY, &cm_equip_manip_menu);
        }
    }

    // handle the input and the menu actions
    switch (cm_state) {
        case INVENTORY_MENU:
            res = update_inventory_menu(input, inv_menu_res);
            break;
        case INV_MANIPULATION:
            if (input == C) {
                cm_state = ABILITY_MENU;
                cm_ability_menu.selected_index = 0;
                clear_screen();
            } else {
                // handle the inventory manipulation menu
                switch (inv_manip_menu_res) {
                    case 0:// equip gear was pressed
                        const gear_t* gear_to_equip = player->vtable->get_gear_at(player, cm_inv_menu.selected_index);
                        // if the same item is already equipped, do nothing
                        if (player->vtable->is_gear_equipped(player, gear_to_equip) == 0) {
                            const int equip_success = player->vtable->equip_gear(player, gear_to_equip);
                            if (equip_success != 0) {
                                //something went wrong!!!
                                log_msg(ERROR, "Character Mode",
                                        "Failed to equip gear %s in `update_character_mode`.",
                                        gear_to_equip->local_name);
                            }
                        }
                        break;
                    case 1:                    // drop gear was pressed
                    case 2:                    // show gear details was pressed
                    case MAX_OPTIONS_INV_MANIP:// nothing was pressed, do nothing
                        break;
                    case -1:// esc was pressed
                        cm_state = INVENTORY_MENU;
                        cm_inv_menu_arg.mode = ACTIVE;
                        clear_screen();
                        break;
                    case -2:
                        res = EXIT_GAME;// exit game
                        break;
                    default:
                        log_msg(WARNING, "Character Mode",
                                "Invalid option returned in `update_character_mode` for inventory manipulation: %d",
                                inv_manip_menu_res);
                        break;
                }
            }
            break;
        case EQUIPMENT_MENU:
            switch (input) {
                case LEFT:
                    cm_state = INVENTORY_MENU;
                    cm_inv_menu_arg.mode = ACTIVE;
                    cm_equip_menu_arg.mode = INACTIVE_WOUT_SEL;
                    break;
                default:
                    break;
            }
            break;
        case EQUIP_MANIPULATION:
            break;
        case ABILITY_MENU:
            switch (input) {
                case C:
                    cm_state = INVENTORY_MENU;
                    cm_inv_menu.selected_index = 0;
                    cm_equip_menu.selected_index = 0;
                    cm_inv_menu_arg.mode = ACTIVE;
                    cm_equip_menu_arg.mode = INACTIVE_WOUT_SEL;
                    clear_screen();
                    break;
                default:
                    break;
            }
            break;
        case EXIT_CHARACTER_MODE:
            res = MAP_MODE;// return to the map mode
            break;
    }

    return res;
}

void shutdown_character_mode(void) {
    for (int i = 0; i < MAX_CHAR_MODE_STRINGS; i++) {
        if (cm_strings[i] != NULL) {
            free(cm_strings[i]);
        }
    }
    free(cm_strings);
}

int update_inventory_menu(const input_t input, const int menu_result) {
    state_t res = CHARACTER_MODE;
    switch (input) {
        case RIGHT:// change to the equipment menu
            cm_state = EQUIPMENT_MENU;
            cm_equip_menu_arg.mode = ACTIVE;
            cm_inv_menu_arg.mode = INACTIVE_WOUT_SEL;
            break;
        case C:// change to the ability overview
            cm_state = ABILITY_MENU;
            cm_ability_menu.selected_index = 0;
            clear_screen();
            break;
        default:// handle the inventory menu as normal
            if (menu_result == cm_inv_menu.option_count) {
                // important edge case, when no gear is in the inventory
                // ... do nothing
            } else if (menu_result >= 0 && menu_result < cm_inv_menu.option_count) {
                // a gear was selected in the inventory menu
                cm_state = INV_MANIPULATION;
                // set the inventory menu to inactive, but the selection should still be printed
                cm_inv_menu_arg.mode = INACTIVE_WITH_SEL;
                cm_inv_manip_menu.selected_index = 0;
                clear_screen();
            } else if (menu_result == -1) {
                // esc was pressed, exiting character mode
                cm_state = EXIT_CHARACTER_MODE;
            } else if (menu_result == -2) {
                // Ctrl + C was pressed, exiting game
                res = EXIT_GAME;
            }
            break;
    }
    return res;
}

void update_character_mode_local(void) {
    for (int i = 0; i < MAX_CHAR_MODE_STRINGS; i++) {
        if (cm_strings[i] != NULL) {
            free(cm_strings[i]);
        }
    }

    cm_strings[CHAR_MODE_TITLE] = get_local_string("CM.TITLE");
    cm_strings[CHANGE_TO_ABILITY_OV] = get_local_string("CM.CHANGE_TO.OVERVIEW.ABILITY");
    cm_strings[CHANGE_TO_INVENTORY_OV] = get_local_string("CM.CHANGE_TO.OVERVIEW.INVENTORY");

    cm_strings[INVENTORY_TITLE] = get_local_string("INVENTORY");
    cm_inv_menu.title = cm_strings[INVENTORY_TITLE];
    cm_strings[EQUIPMENT_TITLE] = get_local_string("EQUIPPED");
    cm_equip_menu.title = cm_strings[EQUIPMENT_TITLE];
    cm_strings[MANIP_TITLE] = get_local_string("CM.MANIP.TITLE");
    cm_inv_manip_menu.title = cm_strings[MANIP_TITLE];
    cm_equip_manip_menu.title = cm_strings[MANIP_TITLE];
    cm_strings[PRESS_ESC_CANCEL] = get_local_string("PRESS_ESC.CANCEL");
    cm_inv_manip_menu.tailing_text = cm_strings[PRESS_ESC_CANCEL];
    cm_equip_manip_menu.tailing_text = cm_strings[PRESS_ESC_CANCEL];

    cm_strings[EMPTY_GEAR_SLOT] = get_local_string("CM.INVENTORY.SLOT.EMPTY");
    cm_strings[HEAD_STR] = get_local_string("HEAD");
    cm_strings[BODY_STR] = get_local_string("BODY");
    cm_strings[LEGS_STR] = get_local_string("LEGS");
    cm_strings[HANDS_STR] = get_local_string("HANDS");
    cm_strings[RING_LEFT_STR] = get_local_string("RING_LEFT");
    cm_strings[RING_RIGHT_STR] = get_local_string("RING_RIGHT");
    cm_strings[AMULET_STR] = get_local_string("AMULET");
    cm_strings[MAIN_HAND_STR] = get_local_string("MAIN_HAND");
    cm_strings[OFF_HAND_STR] = get_local_string("OFF_HAND");

    cm_strings[EQUIP_GEAR_STR] = get_local_string("CM.INVENTORY.GEAR.EQUIP");
    cm_strings[DROP_GEAR_STR] = get_local_string("CM.INVENTORY.GEAR.DROP");
    cm_strings[SHOW_GEAR_DETAILS_STR] = get_local_string("CM.INVENTORY.GEAR.SHOW_DETAILS");
    cm_strings[UNEQUIP_GEAR_STR] = get_local_string("CM.INVENTORY.GEAR.UNEQUIP");
}
