#include "map_mode.h"

#include "../../game_data/map/map_parser.h"
#include "../../game_data/map/map_revealer.h"
#include "../../io/output/common/common_output.h"
#include "../../io/output/specific/map_output.h"
#include "src/logger/logger.h"

state_t update_map_mode(const input_t input, map_t* map) {
    state_t next_state = MAP_MODE;

    const int player_on_map_idx = map->player_pos.dx * map->height + map->player_pos.dy;

    switch (input) {
        case UP:
            if (map->player_pos.dy > 0 && map->revealed_tiles[player_on_map_idx - 1] != WALL) {
                map->player_pos.dy--;
            }
            break;
        case DOWN:
            if (map->player_pos.dy < map->height - 1 && map->revealed_tiles[player_on_map_idx + 1] != WALL) {
                map->player_pos.dy++;
            }
            break;
        case LEFT:
            if (map->player_pos.dx > 0 && map->revealed_tiles[player_on_map_idx - map->height] != WALL) {
                map->player_pos.dx--;
            }
            break;
        case RIGHT:
            if (map->player_pos.dx < map->width - 1 && map->revealed_tiles[player_on_map_idx + map->height] != WALL) {
                map->player_pos.dx++;
            }
            break;
        case M:
            //no break needed
        case ESCAPE:
            next_state = SETTINGS; //open settings
        case I:
            next_state = INVENTORY_MODE; // open inventory
            break;
        case C:
            next_state = CHARACTER_MODE; // open character screen
            break;
        case QUIT:
            next_state = EXIT_GAME; // exit game
            break;
        default:
            //does nothing
            break;
    }
    reveal_map(map, 3);

    clear_screen();
    parsed_map_t* parsed_map = parse_map(map->width, map->height, map->revealed_tiles, map->player_pos);
    RETURN_WHEN_NULL(parsed_map, EXIT_GAME, "Map Mode", "Failed to parse map");
    print_map(5, 5, parsed_map);

    free(parsed_map);

    return next_state;
}
