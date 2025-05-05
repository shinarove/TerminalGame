#ifndef COMBAT_MODE_H
#define COMBAT_MODE_H

#include "../../game.h"
#include "../../io/input/input_handler.h"

int init_combat_mode(void);

state_t update_combat_mode(input_t input, character_t* player);

void shutdown_combat_mode(void);

#endif//COMBAT_MODE_H
