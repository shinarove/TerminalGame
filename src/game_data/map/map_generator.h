#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include "../../memory/mem_mgmt.h"
#include "map.h"

/**
 * Generates a map using a specified memory pool and configuration.
 *
 * @param pool Pointer to the memory pool to be used for memory allocations during map generation.
 * @param map_to_generate Pointer to the map structure to be filled with the generated map data.
 * @param generate_exit Flag indicating whether an exit point should be included in the generated map.
 *                      A non-zero value enables the generation of an exit point, while zero disables it.
 * @return Returns an integer indicating the success or failure of the map generation process.
 *         Typically, 0 represents success, whereas a non-zero value indicates an error.
 */
int generate_map(const memory_pool_t* pool, map_t* map_to_generate, int generate_exit);

#endif//MAP_GENERATOR_H
