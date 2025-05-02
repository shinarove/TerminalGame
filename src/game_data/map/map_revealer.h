#ifndef MAP_REVEALER_H
#define MAP_REVEALER_H

#include "map.h"

/**
 * Reveals the tiles within a given light radius around the player's position
 * on the provided map. The method ensures tiles within the revealed radius
 * are updated from hidden to visible on the `revealed_tiles` array.
 *
 * @param map_to_reveal Pointer to the map structure representing the current
 *        game or environment. It contains the player's position, dimensions,
 *        and the hidden and revealed tile representations.
 * @param light_radius The radius of light or visibility to be applied from
 *        the player's position. This must be a positive integer.
 * @return Returns 0 if the operation is successful, or an error code if
 *         invalid inputs are detected (e.g., null pointers, invalid map
 *         dimensions, or non-positive light radius).
 */
int reveal_map(const map_t* map_to_reveal, int light_radius);

#endif//MAP_REVEALER_H
