#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include "../../memory/mem_mgmt.h"
#include "map.h"

/**
 * @brief Generates a 2D map with predefined dimensions, initializes its tiles,
 *        and creates a valid layout using a depth-first search (DFS) algorithm.
 *
 * @param pool Pointer to a memory pool (memory_pool_t) for memory allocation.
 *             This pool is used to allocate memory for the map's tiles.
 * @param map_to_generate Pointer to the map_t structure where the generated map
 *                        data will be stored. This function can modify the height,
 *                        width, tiles, and other attributes of the map.
 *
 * @return Returns 0 on successful map generation or 1 on failure due to invalid
 *         inputs or memory allocation failures.
 *
 * @note If the dimensions of the map are too small, default dimensions are applied.
 *       If the dimensions are not odd, they are incremented to ensure odd values.
 * @note The function uses DFS to generate paths on the map and ensures there is
 *       both a start and an exit position with valid coordinates.
 * @note Memory for `hidden_tiles` and `revealed_tiles` in the map is allocated
 *       using the provided memory pool. If memory allocation fails, the function
 *       terminates with an error.
 * @warning Ensure the provided memory pool is initialized and large enough
 *          to allocate the required memory for the map tiles.
 */
int generate_map(const memory_pool_t* pool, map_t* map_to_generate);

#endif//MAP_GENERATOR_H
