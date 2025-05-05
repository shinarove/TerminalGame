#include "combat_mode.h"

#include "../../io/local/local_handler.h"
#include "../../logger/logger.h"

#include <stdlib.h>

enum combat_mode_index {
    MESSAGE1,
    MAX_COMBAT_MODE_INDEX
};

char** combat_mode_strings = NULL;

void update_combat_mode_local(void);

int init_combat_mode() {
    combat_mode_strings = (char**) malloc(sizeof(char*) * MAX_COMBAT_MODE_INDEX);
    RETURN_WHEN_NULL(combat_mode_strings, 1, "Combat Mode", "Failed to allocate memory for combat mode strings.");

    for (int i = 0; i < MAX_COMBAT_MODE_INDEX; i++) {
        combat_mode_strings[i] = NULL;
    }

    update_combat_mode_local();
    observe_local(update_combat_mode_local);
    return 0;
}

state_t update_combat_mode(input_t input, character_t* player) {
    return COMBAT_MODE;
}

void shutdown_combat_mode() {
    if (combat_mode_strings != NULL) {
        for (int i = 0; i < MAX_COMBAT_MODE_INDEX; i++) {
            free(combat_mode_strings[i]);
        }
        free(combat_mode_strings);
    }
}
void update_combat_mode_local(void) {
}
