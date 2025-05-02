#ifndef MAP_OUTPUT_H
#define MAP_OUTPUT_H

#include "../../../game_data/map/map_parser.h"

/**
 * Prints the provided map on the console, starting from the specified anchor point.
 *
 * @param x The x-coordinate of the anchor point where the map will be printed.
 * @param y The y-coordinate of the anchor point where the map will be printed.
 * @param map A pointer to the parsed map tile structure containing the tile symbols and their associated colors.
 */
void print_map(int x, int y, const parsed_map_t* map);

#endif//MAP_OUTPUT_H
