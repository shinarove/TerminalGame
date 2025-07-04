#include "enemy_generator.h"

#include "../../logger/logger.h"
#include "../ability/ability.h"
#include "enemy_id.h"
#include "goblin.h"

#include <stdio.h>

Character* generate_goblin(int level) {
    level = (level < 1 ? 1 : level) > 20 ? 20 : level;//level can only be between 1 and 20
    char goblin_name[32];
    snprintf(goblin_name, sizeof(goblin_name), "GOBLIN.TYPE%d.NAME", level / 6);

    Character* goblin = create_base_character(GOBLIN, goblin_name);
    RETURN_WHEN_NULL(goblin, NULL, "Enemy Generator", "Failed to allocate memory for goblin")

    //level up goblin
    for (int i = 1; i < level; i++) {
        goblin->vtable->lvl_up(goblin, lvl_table_goblin[i]);
    }

    //TODO: add random items

    return goblin;
}
