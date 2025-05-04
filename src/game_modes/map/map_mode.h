#ifndef MAP_MODE_H
#define MAP_MODE_H

#include "../../game.h"
#include "../../io/input/input_handler.h"
#include "../../game_data/map/map.h"

/**
 * Updates the game state and map based on player input during map mode.
 *
 * This function processes the player's input, updates the player's position on the map,
 * and determines the next game state, such as switching to inventory or settings mode.
 * It also triggers map revealing and renders the map on the screen.
 *
 * @param input The player's input determining the action to perform.
 * @param map A pointer to the map structure that holds the map data, including the player's position,
 *            map dimensions, and revealed tiles.
 * @return The next state of the game, determined by the player's input.
 */
state_t update_map_mode(input_t input, map_t* map);

#endif//MAP_MODE_H
