#ifndef MAP_EVENT_HANDLER_H
#define MAP_EVENT_HANDLER_H

#include "../../game.h"
#include "../../game_data/character/character.h"
#include "../../game_data/map/map.h"

/**
 * Handles events related to the provided map and updates the player's state accordingly.
 *
 * This function processes events occurring within the specified map, including interactions
 * with environmental objects such as fountains. It may alter the player's state or resources
 * based on the triggered events. If the map or player is null, the function logs the error
 * and takes no further action.
 *
 * @param map A pointer to the map structure representing the current map context.
 * @param player A pointer to the character structure representing the player. The player's
 *               attributes and resources may be updated based on the triggered events.
 * @return The state_t value representing the resulting game state after handling the event.
 *         Typically, this remains as MAP_MODE unless modified within the function.
 */
state_t handle_map_event(map_t* map, character_t* player);

#endif//MAP_EVENT_HANDLER_H
