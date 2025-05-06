#ifndef COMBAT_MODE_H
#define COMBAT_MODE_H

#include "../../game.h"
#include "../../io/input/input_handler.h"

int init_combat_mode(void);

state_t prepare_combat_mode(const character_t* player, const character_t* enemy);

state_t update_combat_mode(input_t input, character_t* player, character_t* enemy);

void free_prepared_resources();

void shutdown_combat_mode(void);

#endif//COMBAT_MODE_H
