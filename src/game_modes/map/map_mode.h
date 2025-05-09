#ifndef MAP_MODE_H
#define MAP_MODE_H

#include "../../game.h"
#include "../../game_data/map/map.h"
#include "../../io/input/input_handler.h"
#include "../../game_data/character/character.h"

/**
 * Initializes the map mode by allocating memory for map mode strings,
 * resetting the map mode strings to null, and setting up local updates.
 *
 * @return 0 if initialization is successful, or 1 if memory allocation for
 *         map mode strings fails.
 */
int init_map_mode(void);

/**
 * Updates the state of the game in map mode by processing player input, moving
 * the player character, revealing the map, handling map events, and rendering
 * the updated map.
 *
 * @param input The input action performed by the player, such as movement or
 *              menu initiation.
 * @param map The current map object containing map details including dimensions,
 *            revealed tiles, and player position.
 * @param player The player character whose state may be affected by map events.
 *
 * @return The next game state, which can remain in map mode or transition to
 *         another state such as the main menu, inventory, character screen, or
 *         exit game.
 */
state_t update_map_mode(input_t input, map_t* map, character_t* player);

/**
 * Shuts down the map mode by freeing allocated memory for all map mode strings and for
 * the map mode strings array itself. If the map mode strings array is already null,
 * the function returns immediately. Only non-null strings within the array are freed.
 *
 * This function ensures proper cleanup of resources related to map mode,
 * preventing memory leaks.
 */
void shutdown_map_mode(void);

#endif//MAP_MODE_H
