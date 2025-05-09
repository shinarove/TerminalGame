#ifndef CHARACTER_CREATION_MODE_H
#define CHARACTER_CREATION_MODE_H

#include "../../game.h"
#include "../../io/input/input_handler.h"
#include "../../game_data/character/character.h"


int init_character_creation_mode(void);

state_t update_character_creation_mode(input_t input, character_t* player);

void shutdown_character_creation_mode(void);

#endif //CHARACTER_CREATION_MODE_H
