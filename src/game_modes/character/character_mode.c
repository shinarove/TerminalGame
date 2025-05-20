#include "character_mode.h"

#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../logger/logger.h"
#include "../../io/output/specific/character_output.h"

#include <stdlib.h>

#define CHARM_Y_POS_PLAYER_HEAD 2

typedef enum {
    INVENTORY_STATE,
    INV_GEAR_MANIPULATION,
    EQUIPMENT_STATE,
    EQUIP_GEAR_MANIPULATION
} char_mode_state_t;

enum character_mode_index {


    MAX_CHARACTER_MODE_STRINGS
};

char** character_mode_strings = NULL;

char_mode_state_t cm_state = INVENTORY_STATE;

void update_character_mode_local(void);

int init_character_mode() {
    character_mode_strings = (char**) malloc(sizeof(char*) * 2);

    update_character_mode_local();
    observe_local(update_character_mode_local);
    return 0;
}

state_t prepare_character_mode(character_t* player) {
    cm_state = INVENTORY_STATE;

    return CHARACTER_MODE;
}

state_t update_character_mode(input_t input, character_t* player) {
    RETURN_WHEN_NULL(player, EXIT_GAME, "Character Mode", "In `update_character_mode` given player is NULL.")

    state_t res = CHARACTER_MODE;

    const output_args_c_t lvl_up_args = {1, 1, 0};
    // print the simple header menu
    print_char_h(5, CHARM_Y_POS_PLAYER_HEAD, player, lvl_up_args);

    return res;
}

void shutdown_character_mode(void) {
}

void update_character_mode_local(void) {

}
