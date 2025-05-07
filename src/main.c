#include "../refterm/fast_pipe.h"
#include "../termbox2/termbox2.h"
#include "game.h"
#include "game_data/ability/ability.h"
#include "game_data/character/character.h"
#include "game_modes/combat/combat_mode.h"
#include "game_modes/map/map_mode.h"
#include "game_modes/menus/language_menu_mode.h"
#include "game_modes/menus/main_menu_mode.h"
#include "game_modes/menus/title_screen_mode.h"
#include "io/input/input_handler.h"
#include "io/local/local_handler.h"
#include "logger/logger.h"
#include "memory/mem_mgmt.h"

#include <stdlib.h>
#include <time.h>

enum exit_codes {
    SUCCESS,
    ERROR_MEMORY_POOL_INIT,
    ERROR_LOCAL_INIT,
    ERROR_MAP_MODE_INIT,
    ERROR_COMBAT_MODE_INIT,
    ERROR_TITLE_SCREEN_INIT,
    ERROR_MAIN_MENU_INIT,
    ERROR_CHANGE_LANGUAGE_INIT,
    ERROR_ABILITY_TABLE_INIT,
};

int init(memory_pool_t** pool);
void shutdown(memory_pool_t** pool);

int init(memory_pool_t** pool) {
    // init core components
    tb_init();
    init_logger();
    init_input_handler();
    *pool = init_memory_pool(2 * STANDARD_MEMORY_POOL_SIZE);
    if (*pool == NULL) return ERROR_MEMORY_POOL_INIT;
    if (init_local_handler(LANGE_EN) != 0) return ERROR_LOCAL_INIT;

    // init of the different modes
    if (init_map_mode() != 0) return ERROR_MAP_MODE_INIT;
    if (init_combat_mode() != 0) return ERROR_COMBAT_MODE_INIT;
    if (init_title_screen() != 0) return ERROR_TITLE_SCREEN_INIT;
    if (init_main_menu() != 0) return ERROR_MAIN_MENU_INIT;
    if (init_change_language() != 0) return ERROR_CHANGE_LANGUAGE_INIT;

    // init game data tables
    if (init_ability_table(*pool) == NULL) return ERROR_ABILITY_TABLE_INIT;

    // Seed the random number generator with a combination of time, process ID, and stack variable address
    unsigned int seed = (unsigned int) time(NULL);// Use current time as seed
    seed ^= (unsigned int) getpid();              // XOR with process ID
    int stack_var;
    seed ^= (unsigned int) &stack_var;// XOR with the address of a stack variable
    srand(seed);                      // Seed the random number generator

    return SUCCESS;
}

int main(void) {
    //only available for win
    USE_FAST_PIPE_IF_AVAILABLE();

    memory_pool_t* pool = NULL;
    const int exit_code = init(&pool);

    character_t* player = create_base_character(pool, 1, "Hero");
    add_ability_c(player, PUNCH);

    if (exit_code == 0) {
        start_game_loop(pool, player);
    }

    destroy_character(pool, player);
    shutdown(&pool);
    return exit_code;
}

void shutdown(memory_pool_t** pool) {
    // shutdown game data tables
    destroy_ability_table(*pool);

    // shutdown the different modes
    shutdown_change_language();
    shutdown_main_menu();
    shutdown_title_screen();
    shutdown_combat_mode();
    shutdown_map_mode();

    // shutdown core components
    shutdown_local_handler();
    shutdown_memory_pool(*pool);
    shutdown_input_handler();
    shutdown_logger();
    tb_shutdown();
}
