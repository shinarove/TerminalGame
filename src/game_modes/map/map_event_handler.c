#include "map_event_handler.h"

#include "../../logger/logger.h"

void handle_fountain_event(const map_t* map, void (*reset_func)(Character*), Character* player);

state_t handle_map_event(map_t* map, Character* player) {
    RETURN_WHEN_NULL(map, MAP_MODE, "Map Event Handler", "Map is NULL")
    RETURN_WHEN_NULL(player, MAP_MODE, "Map Event Handler", "Player is NULL")

    const int player_on_map_idx = map->player_pos.dx * map->height + map->player_pos.dy;
    const map_tile_t tile = map->hidden_tiles[player_on_map_idx];

    state_t next_state = MAP_MODE;
    switch (tile) {
        case START_DOOR:
            next_state = ENTER_PREV_FLOOR;
            break;
        case EXIT_DOOR:
            if (map->exit_unlocked) {
                next_state = ENTER_NEXT_FLOOR;
            } else if (player->has_map_key) {
                player->has_map_key = 0;
                map->exit_unlocked = 1;
                next_state = ENTER_NEXT_FLOOR;
            }
            break;
        case DOOR_KEY:
            player->has_map_key = 1;
            map->hidden_tiles[player_on_map_idx] = FLOOR;
            map->revealed_tiles[player_on_map_idx] = FLOOR;
            break;
        case LIFE_FOUNTAIN:
            handle_fountain_event(map, player->vtable->reset_health, player);
            break;
        case STAMINA_FOUNTAIN:
            handle_fountain_event(map, player->vtable->reset_stamina, player);
            break;
        case MANA_FOUNTAIN:
            handle_fountain_event(map, player->vtable->reset_mana, player);
            break;
        case ENEMY:
            next_state = GENERATE_ENEMY;
            map->hidden_tiles[player_on_map_idx] = FLOOR;
            map->revealed_tiles[player_on_map_idx] = FLOOR;
            break;
        default:
            // do nothing
            break;
    }
    return next_state;
}

void handle_fountain_event(const map_t* map, void (*reset_func)(Character*), Character* player) {
    const int player_on_map_idx = map->player_pos.dx * map->height + map->player_pos.dy;

    reset_func(player);
    map->hidden_tiles[player_on_map_idx] = FLOOR;
    map->revealed_tiles[player_on_map_idx] = FLOOR;
}
