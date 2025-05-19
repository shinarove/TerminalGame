#include "character_creation_mode.h"

#include "../../game_data/character/stats.h"
#include "../../io/colors.h"
#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../logger/logger.h"
#include "../../io/output/specific/character_output.h"

#define CC_Y_POS_HEAD 2
#define CC_Y_POS_UNSPENT_P 6
#define CC_Y_POS_BODY 8

#define CLEAR_X_START 25
#define CLEAR_X_END 45

#define MAX_OPTION_LENGTH 14
#define MAX_NAME_LENGTH 32

#define UNSPENT_RES_POINTS 10
#define UNSPENT_ATTR_POINTS 5

#define UNSPENT_POINTS_FORMAT "%s: %d   "

typedef enum {
    PRE_CREATION,
    NAME_INPUT,
    RESSOURCE_DISTRIBUTION,
    ATTRIBUTE_DISTRIBUTION,
    WAIT_AFTER_CREATION
} cc_mode_state_t;

enum cc_mode_index {
    // strings that are updated via local
    INTRODUCTION_TEXT,
    UNPENT_POINTS_TEXT,
    SPEND_RES_P_TEXT,
    SPEND_ATTR_P_TEXT,
    STRENGTH_STR,
    INTELLIGENCE_STR,
    AGILITY_STR,
    CONSTITUTION_STR,
    LUCK_STR,
    LEVEL_STR,
    HEALTH_STR,
    STAMINA_STR,
    MANA_STR,
    CONTINUE_ENTER,
    CONFIRM_ENTER,
    CONFIRM_Y,
    // strings that are updated after an action
    UNSPENT_POINTS_FULL,
    MAX_CC_STRINGS
};

char** cc_mode_strings = NULL;

cc_mode_state_t cc_state = PRE_CREATION;

menu_t spend_res_p_menu;
menu_t spend_attr_p_menu;
spinner_menu_t spend_res_p_spinner;
spinner_menu_t spend_attr_p_spinner;
menu_arg_t cc_menu_args;

char* name_input_buffer = NULL;

void update_cc_local(void);

void update_stats(int bool_exp, int* updated_stats, int* unspent_points, int diff, const character_t* player);

void update_spent_p_str(int unspent_points);

int init_character_creation(void) {
    cc_mode_strings = (char**) malloc(sizeof(char*) * MAX_CC_STRINGS);
    RETURN_WHEN_NULL(cc_mode_strings, 1, "Character Creation", "Failed to allocate memory for cc_mode_strings");

    for (int i = 0; i < MAX_CC_STRINGS; i++) {
        cc_mode_strings[i] = NULL;
    }
    // initialize the menu: resources
    spend_res_p_menu.options = &cc_mode_strings[HEALTH_STR];
    spend_res_p_menu.option_count = MAX_RESOURCES;
    spend_res_p_menu.selected_index = 0;
    spend_res_p_menu.tailing_text = " ";
    spend_res_p_menu.args = &cc_menu_args;
    // initialize the menu: attributes
    spend_attr_p_menu.options = &cc_mode_strings[STRENGTH_STR];
    spend_attr_p_menu.option_count = MAX_ATTRIBUTES;
    spend_attr_p_menu.selected_index = 0;
    spend_attr_p_menu.tailing_text = " ";
    spend_attr_p_menu.args = &cc_menu_args;
    // initialize the spinner menu: resources
    spend_res_p_spinner.menu = &spend_res_p_menu;
    spend_res_p_spinner.left_symbol = '<';
    spend_res_p_spinner.right_symbol = '>';
    spend_res_p_spinner.max_option_length = MAX_OPTION_LENGTH;
    // initialize the spinner menu: attributes
    spend_attr_p_spinner.menu = &spend_attr_p_menu;
    spend_attr_p_spinner.left_symbol = '<';
    spend_attr_p_spinner.right_symbol = '>';
    spend_attr_p_spinner.max_option_length = MAX_OPTION_LENGTH;
    // initialize the menu args
    cc_menu_args.active = 1;
    cc_menu_args.unselected_fg = WHITE;
    cc_menu_args.unselected_bg = DEFAULT;
    cc_menu_args.selected_fg = BLACK;
    cc_menu_args.selected_bg = WHITE;

    update_cc_local();
    observe_local(update_cc_local);
    return 0;
}

state_t update_character_creation(const input_t input, character_t* player) {
    RETURN_WHEN_NULL(player, EXIT_GAME, "Character Creation", "In `update_character_creation` given player is NULL.");
    state_t res = CHARACTER_CREATION;

    const output_args_c_t args_no_update = {false, false, false};
    const output_args_c_t args_update = {true, false, false};

    switch (cc_state) {
        case PRE_CREATION:
            player->id = 0;
            player->unspent_attr_p = UNSPENT_ATTR_POINTS;
            player->unspent_res_p = UNSPENT_RES_POINTS;
            cc_state = NAME_INPUT;
            break;
        case NAME_INPUT:
            if (name_input_buffer == NULL) {// start text input if not already started
                name_input_buffer = start_text_input(MAX_NAME_LENGTH);
            }
            print_text(5, 2, WHITE, DEFAULT, cc_mode_strings[INTRODUCTION_TEXT]);
            print_text(5, 4, WHITE, DEFAULT, ">");

            if (name_input_buffer[0] != '\0') {
                // input buffer is not empty, show text: "Press ENTER to confirm"
                print_text(5, 6, WHITE, DEFAULT, cc_mode_strings[CONFIRM_ENTER]);
            } else {
                clear_line(6, 5, CLEAR_X_END);
            }

            switch (input) {
                case BACKSPACE:
                    clear_line(4, 7, CLEAR_X_END);
                    break;
                case ENTER:
                    if (name_input_buffer[0] != '\0') {
                        player->name = strdup(name_input_buffer);
                        end_text_input();
                        name_input_buffer = NULL;

                        update_spent_p_str(player->unspent_res_p);
                        cc_state = RESSOURCE_DISTRIBUTION;
                        clear_screen();
                        // print the character info (with update) once before ressource distribution
                        print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    }
                    break;
                case QUIT:
                    res = EXIT_GAME;
                    break;
                default:
                    break;
            }
            // print the name
            print_text(7, 4, WHITE, DEFAULT, name_input_buffer);
            break;
        case RESSOURCE_DISTRIBUTION:
            switch (handle_spinner_menu(input, 5, CC_Y_POS_BODY, &spend_res_p_spinner)) {
                case 0:// decrease health by one
                    update_stats(player->base_resources.health > 1,
                                 &player->base_resources.health, &player->unspent_res_p, -1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 1:// increase health by one
                    update_stats(player->unspent_res_p > 0,
                                 &player->base_resources.health, &player->unspent_res_p, 1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 2:// decrease stamina by one
                    update_stats(player->base_resources.stamina > 1,
                                 &player->base_resources.stamina, &player->unspent_res_p, -1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 3:// increase stamina by one
                    update_stats(player->unspent_res_p > 0,
                                 &player->base_resources.stamina, &player->unspent_res_p, 1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 4:// decrease mana by one
                    update_stats(player->base_resources.mana > 1,
                                 &player->base_resources.mana, &player->unspent_res_p, -1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 5:// increase mana by one
                    update_stats(player->unspent_res_p > 0,
                                 &player->base_resources.mana, &player->unspent_res_p, 1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case MAX_RESOURCES * 2:// nothing was pressed, do nothing
                case -1:               // ESC was pressed, do nothing
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_no_update);
                    break;
                case -2:// Ctrl + C was pressed
                    res = EXIT_GAME;
                    break;
                default:
                    log_msg(ERROR, "Character Creation",
                            "Invalid option returned in `update_character_creation`: %d",
                            spend_res_p_menu.selected_index);
                    break;
            }

            print_text(5, CC_Y_POS_UNSPENT_P, WHITE, DEFAULT, cc_mode_strings[UNSPENT_POINTS_FULL]);

            if (player->unspent_res_p == 0) {
                if (spend_res_p_menu.tailing_text[0] == ' ') {
                    spend_res_p_menu.tailing_text = cc_mode_strings[CONFIRM_Y];
                }

                if (input == Y) {
                    // copy the base resources to max & current resources
                    player->max_resources = player->base_resources;
                    player->current_resources = player->base_resources;

                    cc_state = ATTRIBUTE_DISTRIBUTION;
                    update_spent_p_str(player->unspent_attr_p);
                    spend_res_p_menu.selected_index = 0;
                    clear_screen();
                }
            } else if (spend_res_p_menu.tailing_text[0] != ' ') {
                clear_line(CC_Y_POS_BODY + 6, 5, CLEAR_X_END);// clear the line with the confirmation text
                spend_res_p_menu.tailing_text = " ";
            }
            break;
        case ATTRIBUTE_DISTRIBUTION:
            switch (handle_spinner_menu(input, 5, CC_Y_POS_BODY, &spend_attr_p_spinner)) {
                case 0:// decrease strength by one
                    update_stats(player->base_attributes.strength > 1,
                                 &player->base_attributes.strength, &player->unspent_attr_p, -1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 1:// increase strength by one
                    update_stats(player->unspent_attr_p > 0,
                                 &player->base_attributes.strength, &player->unspent_attr_p, 1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 2:// decrease intelligence by one
                    update_stats(player->base_attributes.intelligence > 1,
                                 &player->base_attributes.intelligence, &player->unspent_attr_p, -1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 3:// increase intelligence by one
                    update_stats(player->unspent_attr_p > 0,
                                 &player->base_attributes.intelligence, &player->unspent_attr_p, 1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 4:// decrease agility by one
                    update_stats(player->base_attributes.agility > 1,
                                 &player->base_attributes.agility, &player->unspent_attr_p, -1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 5:// increase agility by one
                    update_stats(player->unspent_attr_p > 0,
                                 &player->base_attributes.agility, &player->unspent_attr_p, 1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 6:// decrease endurance by one
                    update_stats(player->base_attributes.constitution > 1,
                                 &player->base_attributes.constitution, &player->unspent_attr_p, -1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 7:// increase endurance by one
                    update_stats(player->unspent_attr_p > 0,
                                 &player->base_attributes.constitution, &player->unspent_attr_p, 1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 8:// decrease luck by one
                    update_stats(player->base_attributes.luck > 1,
                                 &player->base_attributes.luck, &player->unspent_attr_p, -1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case 9:// increase luck by one
                    update_stats(player->unspent_attr_p > 0,
                                 &player->base_attributes.luck, &player->unspent_attr_p, 1, player);
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_update);
                    break;
                case MAX_ATTRIBUTES * 2:// nothing was pressed, do nothing
                case -1:                // ESC was pressed, do nothing
                    print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_no_update);
                    break;
                case -2:// Ctrl + C was pressed
                    res = EXIT_GAME;
                    break;
                default:
                    log_msg(ERROR, "Character Creation",
                            "Invalid option returned in `update_character_creation`: %d",
                            spend_attr_p_menu.selected_index);
                    break;
            }

            print_text(5, CC_Y_POS_UNSPENT_P, WHITE, DEFAULT, cc_mode_strings[UNSPENT_POINTS_FULL]);

            if (player->unspent_attr_p == 0) {
                if (spend_attr_p_menu.tailing_text[0] == ' ') {
                    spend_attr_p_menu.tailing_text = cc_mode_strings[CONFIRM_Y];
                }

                if (input == Y) {
                    // copy the base resources to max & current resources
                    player->max_attributes = player->base_attributes;
                    player->current_attributes = player->base_attributes;

                    cc_state = WAIT_AFTER_CREATION;
                    spend_attr_p_menu.selected_index = 0;
                    clear_screen();
                }
            } else if (spend_attr_p_menu.tailing_text[0] != ' ') {
                clear_line(CC_Y_POS_BODY + 8, 5, CLEAR_X_END);// clear the line with the confirmation text
                spend_attr_p_menu.tailing_text = " ";
            }

            break;
        case WAIT_AFTER_CREATION:
            print_c_res_attr_hori(5, CC_Y_POS_HEAD, player, args_no_update);
            print_text(5, CC_Y_POS_BODY, WHITE, DEFAULT, cc_mode_strings[CONTINUE_ENTER]);

            if (input == ENTER) {
                add_ability_c(player, PUNCH);// add the default ability

                cc_state = PRE_CREATION;// reset the cc state
                res = GENERATE_MAP;
                clear_screen();
            }
            break;
    }

    return res;
}

void shutdown_character_creation(void) {
    if (cc_mode_strings != NULL) {
        for (int i = 0; i < MAX_CC_STRINGS; i++) {
            if (cc_mode_strings[i] != NULL) free(cc_mode_strings[i]);
        }
        free(cc_mode_strings);
    }
}


void update_cc_local(void) {
    // free the previous local strings
    for (int i = 0; i < UNSPENT_POINTS_FULL; i++) {
        if (cc_mode_strings[i] != NULL) {
            free(cc_mode_strings[i]);
        }
    }

    cc_mode_strings[INTRODUCTION_TEXT] = get_local_string("CC.INTRODUCTION");
    cc_mode_strings[UNPENT_POINTS_TEXT] = get_local_string("CC.POINTS.UNSPENT");
    cc_mode_strings[SPEND_RES_P_TEXT] = get_local_string("CC.RESOURCES.SPEND.TEXT");
    spend_res_p_menu.title = cc_mode_strings[SPEND_RES_P_TEXT];
    cc_mode_strings[SPEND_ATTR_P_TEXT] = get_local_string("CC.ATTRIBUTES.SPEND.TEXT");
    spend_attr_p_menu.title = cc_mode_strings[SPEND_ATTR_P_TEXT];

    cc_mode_strings[STRENGTH_STR] = get_local_string("STRENGTH");
    cc_mode_strings[INTELLIGENCE_STR] = get_local_string("INTELLIGENCE");
    cc_mode_strings[AGILITY_STR] = get_local_string("AGILITY");
    cc_mode_strings[CONSTITUTION_STR] = get_local_string("CONSTITUTION");
    cc_mode_strings[LUCK_STR] = get_local_string("LUCK");
    cc_mode_strings[LEVEL_STR] = get_local_string("LEVEL");
    cc_mode_strings[HEALTH_STR] = get_local_string("HEALTH");
    cc_mode_strings[STAMINA_STR] = get_local_string("STAMINA");
    cc_mode_strings[MANA_STR] = get_local_string("MANA");

    cc_mode_strings[CONTINUE_ENTER] = get_local_string("PRESS_ENTER.CONTINUE");
    cc_mode_strings[CONFIRM_ENTER] = get_local_string("PRESS_ENTER.CONFIRM");
    cc_mode_strings[CONFIRM_Y] = get_local_string("PRESS_Y.CONFIRM");
}

void update_stats(const int bool_exp, int* updated_stats, int* unspent_points, const int diff, const character_t* player) {
    if (bool_exp) {
        *updated_stats += diff;
        *unspent_points -= diff;
        update_spent_p_str(*unspent_points);
    }
}

void update_spent_p_str(const int unspent_points) {
    // free the previous unspent points string
    if (cc_mode_strings[UNSPENT_POINTS_FULL] != NULL) {
        free(cc_mode_strings[UNSPENT_POINTS_FULL]);
    }

    char* unspent_points_str = malloc(64);
    snprintf(unspent_points_str, 64, UNSPENT_POINTS_FORMAT,
             cc_mode_strings[UNPENT_POINTS_TEXT], unspent_points);
    cc_mode_strings[UNSPENT_POINTS_FULL] = unspent_points_str;
}
