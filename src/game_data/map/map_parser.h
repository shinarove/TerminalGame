#ifndef MAP_PARSER_H
#define MAP_PARSER_H

#include "map.h"

typedef struct {
    char symbol;
    color_t foreground_color;
    color_t background_color;
} parsed_map_t;


/**
 * Parses a given map defined by its width, height, and tiles into a formatted map representation.
 *
 * This function converts a 2D array of map tiles into a more descriptive format, including
 * each tile's symbol and its foreground and background colors, represented as a parsed_map_t structure.
 * The generated formatted map includes newline characters for proper row separation
 * and a null-terminator at the end.
 *
 * @param width The width of the map to parse. Must be greater than 0.
 * @param height The height of the map to parse. Must be greater than 0.
 * @param map_to_parse A pointer to the array of map tiles (map_tile_t) to be parsed.
 *                     Must not be NULL.
 *
 * @return A dynamically allocated pointer to the parsed_map_t array representing the formatted map.
 *         Returns NULL if any input is invalid or memory allocation fails.
 */
parsed_map_t* parse_map(int width, int height, const map_tile_t* map_to_parse);

#endif //MAP_PARSER_H
