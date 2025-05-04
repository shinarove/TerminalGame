#include "map_event_handler.h"

#include "../../logger/logger.h"

/**
 * Handles fountain events on the map, replenishing the player's health, mana, or stamina
 * based on the type of fountain they interact with. The fountain tile is then
 * replaced with a FLOOR tile to indicate it has been used.
 *
 * @param map A pointer to the map structure containing the player's position and tiles information.
 * @param player A pointer to the character structure representing the player whose resources may be affected.
 */
void handle_fountain_event(const map_t* map, character_t* player);

state_t handle_map_event(const map_t* map, character_t* player) {
    RETURN_WHEN_NULL(map, , "Map Event Handler", "Map is NULL");
    RETURN_WHEN_NULL(player, , "Map Event Handler", "Player is NULL");

    handle_fountain_event(map, player);
    return MAP_MODE;
}

void handle_fountain_event(const map_t* map, character_t* player) {
    const int player_on_map_idx = map->player_pos.dx * map->height + map->player_pos.dy;
    const map_tile_t tile = map->hidden_tiles[player_on_map_idx];

    switch (tile) {
        case LIFE_FOUNTAIN:
            reset_health_c(player);
            map->hidden_tiles[player_on_map_idx] = FLOOR;
            map->revealed_tiles[player_on_map_idx] = FLOOR;
            break;
        case MANA_FOUNTAIN:
            reset_mana_c(player);
            map->hidden_tiles[player_on_map_idx] = FLOOR;
            map->revealed_tiles[player_on_map_idx] = FLOOR;
            break;
        case STAMINA_FOUNTAIN:
            reset_stamina_c(player);
            map->hidden_tiles[player_on_map_idx] = FLOOR;
            map->revealed_tiles[player_on_map_idx] = FLOOR;
            break;
        default:
            // do nothing
            break;
    }
}
