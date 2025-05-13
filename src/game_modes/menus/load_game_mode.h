#ifndef LOAD_GAME_MODE_H
#define LOAD_GAME_MODE_H

#include "../../game.h"
#include "../../game_data/save_file_handler.h"
#include "../../io/input/input_handler.h"
#include "../../memory/mem_mgmt.h"


int init_load_game_mode();

state_t prepare_load_game_mode(memory_pool_t* pool, game_state_t* game_state);

state_t update_load_game_mode(input_t input, state_t called_from);

void shutdown_load_game_mode(void);

#endif//LOAD_GAME_MODE_H
