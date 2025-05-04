#include "map_parser.h"

#include "../../logger/logger.h"

#include <stdlib.h>

parsed_map_t* create_parsed_map(const int width, const int height, const map_tile_t* map_to_parse, const vector2d_t player_pos) {
    RETURN_WHEN_NULL(map_to_parse, NULL, "Map Parser", "Map to parse is NULL");
    RETURN_WHEN_TRUE(width <= 0, NULL, "Map Parser", "Width must be greater than 0");
    RETURN_WHEN_TRUE(height <= 0, NULL, "Map Parser", "Height must be greater than 0");

    // allocate memory for the parsed map
    parsed_map_t* parsed_map = (parsed_map_t*) malloc(sizeof(parsed_map_t));
    RETURN_WHEN_NULL(parsed_map, NULL, "Map Parser", "Failed to allocate memory for parsed map");

    // allocate memory for the map-tiles
    parsed_map_tile_t* parsed_map_tile = (parsed_map_tile_t*) malloc(sizeof(parsed_map_tile_t) * width * height);
    if (parsed_map_tile == NULL) {
        log_msg(ERROR, "Map Parser", "Failed to allocate memory for parsed map");
        free(parsed_map);
        return NULL;
    }

    parsed_map->width = width;
    parsed_map->height = height;
    parsed_map->tiles = parsed_map_tile;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            // get the tile type
            map_tile_t tile = map_to_parse[x * height + y];
            if (x == player_pos.dx && y == player_pos.dy) {
                // if the tile is the player's position, set it to PLAYER
                tile = PLAYER;
            }

            const char symbol = tiles_mapping[tile].symbol;
            const color_t foreground_color = tiles_mapping[tile].foreground_color;
            const color_t background_color = tiles_mapping[tile].background_color;

            // set the symbol and colors
            parsed_map->tiles[x * height + y].symbol = symbol;
            parsed_map->tiles[x * height + y].foreground_color = foreground_color;
            parsed_map->tiles[x * height + y].background_color = background_color;
        }
    }

    return parsed_map;
}
