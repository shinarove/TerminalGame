#ifndef MAP_MODE_H
#define MAP_MODE_H

#include "../../game.h"
#include "../../game_data/map/map.h"
#include "../../io/input/input_handler.h"

/**
 * Initializes the map mode by allocating memory for map mode strings,
 * resetting the map mode strings to null, and setting up local updates.
 *
 * @return 0 if initialization is successful, or 1 if memory allocation for
 *         map mode strings fails.
 */
int init_map_mode(void);

/**
 * Updates the map mode based on player input, processes player movement,
 * handles map events, and transitions to the appropriate game state.
 *
 * @param input The player's input to be processed, determining movement,
 *              menu access, or state transitions.
 * @param map A pointer to the map structure containing the current map
 *            state, including the player's position and revealed tiles.
 * @param player A pointer to the character structure representing the player's
 *               character interacting with the map.
 * @return The next game state after processing the input. This could remain
 *         as MAP_MODE or transition to other states such as MAIN_MENU,
 *         INVENTORY_MODE, CHARACTER_MODE, or EXIT_GAME.
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
