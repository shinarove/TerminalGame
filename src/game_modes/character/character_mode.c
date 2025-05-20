#include "character_mode.h"

#include "../../io/local/local_handler.h"
#include "../../io/menu.h"
#include "../../logger/logger.h"
#include "../../io/output/specific/character_output.h"

#include <stdlib.h>

#define CM_Y_POS_TITLE 2
#define CM_Y_POS_PLAYER_HEAD 4

typedef enum {
    INVENTORY_STATE,
    INV_GEAR_MANIPULATION,
    EQUIPMENT_STATE,
    EQUIP_GEAR_MANIPULATION,
    EXIT_CHARACTER_MODE,
} char_mode_state_t;

enum cm_index {
    CHAR_MODE_TITLE,

    MAX_CHAR_MODE_STRINGS
};

char** cm_strings = NULL;

char_mode_state_t cm_state = INVENTORY_STATE;

void update_character_mode_local(void);

int init_character_mode() {
    cm_strings = (char**) malloc(sizeof(char*) * MAX_CHAR_MODE_STRINGS);
    RETURN_WHEN_NULL(cm_strings, 1, "Character Mode", "Failed to allocate memory for character mode strings.")

    for (int i = 0; i < MAX_CHAR_MODE_STRINGS; i++) {
        cm_strings[i] = NULL;
    }

    update_character_mode_local();
    observe_local(update_character_mode_local);
    return 0;
}

state_t update_character_mode(input_t input, character_t* player) {
    RETURN_WHEN_NULL(player, EXIT_GAME, "Character Mode", "In `update_character_mode` given player is NULL.")

    state_t res = CHARACTER_MODE;

    const output_args_c_t lvl_up_args = {0, RES_CURR_MAX, ATTR_MAX_BONUS};
    print_char_h(5, CM_Y_POS_PLAYER_HEAD, player, lvl_up_args);

    switch (cm_state) {
        case INVENTORY_STATE:
        case INV_GEAR_MANIPULATION:
        case EQUIPMENT_STATE:
        case EQUIP_GEAR_MANIPULATION:
            break;
        case EXIT_CHARACTER_MODE:
            cm_state = INVENTORY_STATE; // reset the state
            res = MAP_MODE; // return to the map mode
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

void update_character_mode_local(void) {
    for (int i = 0; i < MAX_CHAR_MODE_STRINGS; i++) {
        if (cm_strings[i] != NULL) {
            free(cm_strings[i]);
        }
    }

    cm_strings[CHAR_MODE_TITLE] = get_local_string("CM.TITLE");
}
