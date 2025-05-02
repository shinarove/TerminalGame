#include "game.h"

#include "game_data/map/map_generator.h"
#include "io/input/input_handler.h"
#include "logger/logger.h"


void start_game_loop(const memory_pool_t* used_pool) {
    RETURN_WHEN_NULL(used_pool, , "Game", "Memory pool is NULL");

    //allocate maps in the memory pool, with max number of maps == 16
    map_t** maps = memory_pool_alloc(used_pool, 16 * sizeof(map_t*));

    bool running = true;
    state_t current = TITLE_SCREEN;

    while (running) {
        input_t input = get_next_input();

        switch (current) {
            case TITLE_SCREEN:
            case MAP_GENERATION:
            case MAP_MODE:
            case COMBAT_MODE:
            case INVENTORY_MODE:
            case CHARACTER_MODE:
            case SETTINGS:
            case GAME_OVER:
            case EXIT_GAME:
                running = false;
                break;
        }
    }

    memory_pool_free(used_pool, maps);
}
