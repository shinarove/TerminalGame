#ifndef ENEMY_GENERATOR_H
#define ENEMY_GENERATOR_H

#include "../../memory/mem_mgmt.h"
#include "character.h"

character_t* generate_goblin(memory_pool_t* pool, int level);

#endif//ENEMY_GENERATOR_H
