#ifndef DUNGEON_CRAWL_H
#define DUNGEON_CRAWL_H

#include "game_data/character/character.h"
#include "memory/mem_mgmt.h"

typedef enum {
    TITLE_SCREEN,
    GENERATE_MAP,
    MAP_MODE,
    COMBAT_MODE,
    INVENTORY_MODE,
    CHARACTER_MODE,
    MAIN_MENU,
    CHANGE_LANGUAGE,
    GAME_OVER,
    EXIT_GAME
} state_t;

void start_game_loop(const memory_pool_t* used_pool, character_t* player);

#endif//DUNGEON_CRAWL_H
