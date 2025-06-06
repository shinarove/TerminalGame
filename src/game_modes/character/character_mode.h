#ifndef CHARACTER_MODE_H
#define CHARACTER_MODE_H

#include "../../game.h"
#include "../../game_data/character/character.h"
#include "../../io/input/input_handler.h"

int init_character_mode(void);

state_t prepare_character_mode(const Character* player);

state_t update_character_mode(input_t input, Character* player);

void shutdown_character_mode(void);

#endif//CHARACTER_MODE_H
