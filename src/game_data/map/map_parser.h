#ifndef MAP_PARSER_H
#define MAP_PARSER_H

#include "map.h"

typedef struct {
    char symbol;
    color_t foreground_color;
    color_t background_color;
} parsed_map_tile_t;

typedef struct {
    int width;
    int height;
    parsed_map_tile_t* tiles;
} parsed_map_t;

/**
 * Parses a 2D map representation based on specified dimensions and player position.
 * Converts map tiles into a structure containing symbol and color information.
 *
 * @param width The width of the map.
 * @param height The height of the map.
 * @param map_to_parse A pointer to the array of map tiles to be parsed.
 * @param player_pos The position of the player on the map as a 2D vector.
 * @return A pointer to the parsed map structure, or NULL if an error occurs (e.g., invalid input or memory allocation failure).
 * @note The caller is responsible for freeing the allocated memory for the parsed map.
 */
parsed_map_t* create_parsed_map(int width, int height, const map_tile_t* map_to_parse, vector2d_t player_pos);

#endif//MAP_PARSER_H
