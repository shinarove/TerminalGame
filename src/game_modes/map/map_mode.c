#include "map_mode.h"

#include "../../game_data/map/map_parser.h"
#include "../../game_data/map/map_revealer.h"
#include "../../io/local/local_handler.h"
#include "../../io/output/common/common_output.h"
#include "../../io/output/specific/map_output.h"
#include "../../logger/logger.h"
#include "map_event_handler.h"
#include "../../io/output/specific/character_output.h"

enum map_mode_index {
    GAME_TITLE,
    MAX_MAP_MODE_INDEX
};

char** map_mode_strings = NULL;

void update_map_mode_local(void);

int init_map_mode() {
    // allocate memory for the local strings
    map_mode_strings = (char**) malloc(sizeof(char*) * MAX_MAP_MODE_INDEX);
    RETURN_WHEN_NULL(map_mode_strings, 1, "Map Mode", "Failed to allocate memory for map mode strings.")

    for (int i = 0; i < MAX_MAP_MODE_INDEX; i++) {
        map_mode_strings[i] = NULL;
    }

    update_map_mode_local();
    observe_local(update_map_mode_local);
    return 0;
}

state_t update_map_mode(const input_t input, map_t* map, Character* player) {
    state_t next_state = MAP_MODE;

    const int player_on_map_idx = map->player_pos.dx * map->height + map->player_pos.dy;

    parsed_map_t* parsed_map = create_parsed_map(map->width, map->height, map->revealed_tiles, map->player_pos);
    RETURN_WHEN_NULL(parsed_map, EXIT_GAME, "Map Mode", "Failed to parse map")

    print_text(5, 2, RED, DEFAULT, map_mode_strings[GAME_TITLE]);
    print_map(5, 4, parsed_map);

    const output_args_c_t map_mode_args = {1, RES_CURR_MAX, ATTR_MAX};
    print_char_v(5 + map->width + 2, 4, player, map_mode_args);

    free(parsed_map->tiles);
    free(parsed_map);

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
        case ESCAPE:
            clear_screen();
            next_state = MAIN_MENU;//open the main menu
            break;
        case I: // open character screen
        case C: // open character screen
            clear_screen();
            next_state = CHARACTER_MODE;
            break;
        case QUIT:
            next_state = EXIT_GAME;// exit game
            break;
        default:
            //does nothing
            break;
    }
    if (input == UP || input == DOWN || input == LEFT || input == RIGHT) {
        reveal_map(map, 3);
        next_state = handle_map_event(map, player);
        if (next_state != MAP_MODE) clear_screen();
    }

    return next_state;
}

void shutdown_map_mode() {
    if (map_mode_strings == NULL) return;

    for (int i = 0; i < MAX_MAP_MODE_INDEX; i++) {
        if (map_mode_strings[i] != NULL) {
            //only free the strings that were allocated
            free(map_mode_strings[i]);
        }
    }
    free(map_mode_strings);
}

void update_map_mode_local() {
    if (map_mode_strings == NULL) return;

    for (int i = 0; i < MAX_MAP_MODE_INDEX; i++) {
        if (map_mode_strings[i] != NULL) {
            //only free the strings that were allocated
            free(map_mode_strings[i]);
        }
    }

    map_mode_strings[GAME_TITLE] = get_local_string("GAME.TITLE");
}
