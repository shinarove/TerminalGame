#include "enemy_generator.h"

#include "../../logger/logger.h"
#include "goblin.h"
#include "../ability/ability.h"

#include <stdio.h>

character_t* generate_goblin(const memory_pool_t* pool, unsigned int level) {
    RETURN_WHEN_NULL(pool, NULL, "Enemy Generator", "Memory pool is NULL")

    level = (level < 1 ? 1 : level) > 20 ? 20 : level;//level can only be between 1 and 20
    char goblin_name[32];
    snprintf(goblin_name, sizeof(goblin_name), "GOBLIN.TYPE%d.NAME", level / 6);

    character_t* goblin = create_base_character(pool, 0, goblin_name);
    RETURN_WHEN_NULL(goblin, NULL, "Enemy Generator", "Failed to allocate memory for goblin")

    //level up goblin
    for (int i = 1; i < level; i++) {
        level_up_c(goblin, lvl_table_goblin[i]);
    }

    add_ability_c(goblin, CLAWS);

    //TODO: add random items

    return goblin;
}
