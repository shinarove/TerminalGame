#ifndef DUNGEON_CRAWL_H
#define DUNGEON_CRAWL_H

#include "memory/mem_mgmt.h"

typedef enum {
    TITLE_SCREEN,
    MAP_GENERATION,
    MAP_MODE,
    COMBAT_MODE,
    INVENTORY_MODE,
    CHARACTER_MODE,
    SETTINGS,
    GAME_OVER,
    EXIT_GAME
} state_t;

void start_game_loop(const memory_pool_t* used_pool);

#endif//DUNGEON_CRAWL_H
