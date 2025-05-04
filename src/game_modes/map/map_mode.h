#ifndef MAP_MODE_H
#define MAP_MODE_H

#include "../../game.h"
#include "../../game_data/map/map.h"
#include "../../io/input/input_handler.h"

int init_map_mode(void);


state_t update_map_mode(input_t input, map_t* map, character_t* player);

void shutdown_map_mode(void);

#endif//MAP_MODE_H
