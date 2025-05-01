#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include "../../memory/mem_mgmt.h"
#include "map.h"

int generate_map(const memory_pool_t* pool, map_t* map_to_generate);

#endif //MAP_GENERATOR_H
