#ifndef LEVEL_UP_MODE_H
#define LEVEL_UP_MODE_H

#include "../../game.h"
#include "../../io/input/input_handler.h"

int init_lvl_up_mode(void);

state_t prepare_lvl_up_mode(const character_t* player);

state_t update_lvl_up_mode(input_t input, character_t* player);

void free_prepared_lum_resources(void);

void shutdown_lvl_up_mode(void);

#endif //LEVEL_UP_MODE_H
