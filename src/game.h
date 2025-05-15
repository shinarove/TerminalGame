#ifndef DUNGEON_CRAWL_H
#define DUNGEON_CRAWL_H

#include "memory/mem_mgmt.h"

typedef enum {
    TITLE_SCREEN,
    GENERATE_MAP,
    GENERATE_ENEMY,
    CHARACTER_CREATION,
    RESTART_GAME,
    SAVE_GAME,
    LOAD_GAME,
    MAP_MODE,
    ENTER_NEXT_FLOOR,
    ENTER_PREV_FLOOR,
    COMBAT_MODE,
    LVL_UP_MODE,
    INVENTORY_MODE,
    CHARACTER_MODE,
    MAIN_MENU,
    LANGUAGE_MODE,
    GAME_OVER,
    EXIT_GAME
} state_t;

/**
 * This global memory pool should be used for all game resources related memory allocation.
 */
extern memory_pool_t* global_memory_pool;

void start_game_loop(const memory_pool_t* used_pool);

#endif//DUNGEON_CRAWL_H
