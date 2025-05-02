#ifndef MAP_OUTPUT_H
#define MAP_OUTPUT_H

#include "../../../game_data/map/map.h"
#include "../../../game_data/map/map_parser.h"

/**
 * Prints the provided map on the console, starting from the specified anchor point.
 *
 * @param anchor The starting position (x, y) on the console where the map will begin drawing.
 * @param map A pointer to the parsed map tile structure containing the tile symbols and their associated colors.
 */
void print_map(vector2d_t anchor, const parsed_map_tile_t* map);

#endif //MAP_OUTPUT_H
