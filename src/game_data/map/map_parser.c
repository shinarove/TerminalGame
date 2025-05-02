#include "map_parser.h"

#include "../../logger/logger.h"

#include <stdlib.h>

parsed_map_t* parse_map(const int width, const int height, const map_tile_t* map_to_parse, const vector2d_t player_pos) {
    RETURN_WHEN_NULL(map_to_parse, NULL, "Map Parser", "Map to parse is NULL");
    RETURN_WHEN_TRUE(width <= 0, NULL, "Map Parser", "Width must be greater than 0");
    RETURN_WHEN_TRUE(height <= 0, NULL, "Map Parser", "Height must be greater than 0");

    // allocate memory for the parsed map
    parsed_map_t* parsed_map = (parsed_map_t*) malloc(sizeof(parsed_map_t));
    RETURN_WHEN_NULL(parsed_map, NULL, "Map Parser", "Failed to allocate memory for parsed map");

    // allocate memory for the map-tiles, including space for newlines and null terminator
    const unsigned int parsed_map_size = sizeof(parsed_map_tile_t) * width * height + height * sizeof(parsed_map_tile_t);
    parsed_map_tile_t* parsed_map_tile = (parsed_map_tile_t*) malloc(parsed_map_size);
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
            parsed_map->tiles[x * (height + 1) + y].symbol = symbol;
            parsed_map->tiles[x * (height + 1) + y].foreground_color = foreground_color;
            parsed_map->tiles[x * (height + 1) + y].background_color = background_color;
        }
        // add a newline character at the end of each row
        // if the last row, set the symbol to '\0'
        if (x != width - 1) {
            parsed_map->tiles[x * (height + 1) + height].symbol = '\n';
        } else {
            parsed_map->tiles[x * (height + 1) + height].symbol = '\0';
        }
        parsed_map->tiles[x * (height + 1) + height].foreground_color = DEFAULT;
        parsed_map->tiles[x * (height + 1) + height].background_color = DEFAULT;

    }

    return parsed_map;
}
