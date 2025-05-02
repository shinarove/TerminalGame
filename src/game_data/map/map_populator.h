#ifndef MAP_POPULATOR_H
#define MAP_POPULATOR_H

#include "map.h"

/**
 * Populates the given map with necessary game elements such as keys, enemies, and fountains.
 * The function validates the map before adding these elements and ensures the map is properly initialized.
 *
 * @param map_to_populate A pointer to the map to be populated.
 * The map must have its hidden_tiles field initialized before calling this function.
 * @return Returns 0 on successful map population.
 * Returns 1 if the map pointer is NULL or if the map is not properly initialized.
 */
int populate_map(map_t* map_to_populate);

#endif//MAP_POPULATOR_H
