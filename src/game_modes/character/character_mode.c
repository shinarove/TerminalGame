#include "character_mode.h"

#include <stdlib.h>

enum character_mode_index {


    MAX_CHARACTER_MODE_STRINGS
};

char** character_mode_strings = NULL;

int init_character_mode() {
    character_mode_strings = (char**) malloc(sizeof(char*) * 2);

    return 0;
}

state_t prepare_character_mode(character_t* player) {
    return CHARACTER_MODE;
}

state_t update_character_mode(input_t input, character_t* player) {
    return CHARACTER_MODE;
}

void shutdown_character_mode(void) {

}
