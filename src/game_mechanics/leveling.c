#include "leveling.h"

#include "../game_data/character/enemy_generator.h"
#include "../game_data/character/enemy_id.h"
#include "../game_data/character/goblin.h"
#include "../logger/logger.h"

void gain_exp(Character* player, const Character* enemy) {
    RETURN_WHEN_NULL(player, , "Leveling", "In `gain_exp` given player is NULL")
    RETURN_WHEN_NULL(enemy, , "Leveling", "In `gain_exp` given enemy is NULL")

    switch (enemy->id) {
        case GOBLIN:
            player->current_exp += exp_table_goblin[enemy->level];
            break;
        default:
            log_msg(WARNING, "Leveling", "In `gain_exp` enemy with id %d is not supported", enemy->id);
            break;
    }
}
