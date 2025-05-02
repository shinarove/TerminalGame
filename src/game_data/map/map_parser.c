#include "map_parser.h"

#include "../../logger/logger.h"

#include <stdlib.h>

parsed_map_t* parse_map(const int width, const int height, const map_tile_t* map_to_parse) {
    RETURN_WHEN_NULL(map_to_parse, NULL, "Map Parser", "Map to parse is NULL");
    RETURN_WHEN_TRUE(width <= 0, NULL, "Map Parser", "Width must be greater than 0");
    RETURN_WHEN_TRUE(height <= 0, NULL, "Map Parser", "Height must be greater than 0");

    // allocate memory for the string, including space for newlines and null terminator
    const int parsed_map_size = width * height * sizeof(parsed_map_t) + height * sizeof(parsed_map_t);
    parsed_map_t* parsed_map = (parsed_map_t*) malloc(parsed_map_size);
    RETURN_WHEN_NULL(parsed_map, NULL, "Map Parser", "Failed to allocate memory for map string");

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            // get the tile type
            const map_tile_t tile = map_to_parse[x * height + y];

            const char symbol = tiles_mapping[tile].symbol;
            const color_t foreground_color = tiles_mapping[tile].foreground_color;
            const color_t background_color = tiles_mapping[tile].background_color;

            // set the symbol and colors
            parsed_map[x * (height + 1) + y].symbol = symbol;
            parsed_map[x * (height + 1) + y].foreground_color = foreground_color;
            parsed_map[x * (height + 1) + y].background_color = background_color;
        }
        // add a newline character at the end of each row
        // if the last row, set the symbol to '\0'
        if (x != width - 1) {
            parsed_map[x * (height + 1) + height].symbol = '\n';
        } else {
            parsed_map[x * (height + 1) + height].symbol = '\0';
        }
        parsed_map[x * (height + 1) + height].foreground_color = DEFAULT;
        parsed_map[x * (height + 1) + height].background_color = DEFAULT;

    }

    return parsed_map;
}
