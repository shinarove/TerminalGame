#include "../termbox2/termbox2.h"
#include "game.h"
#include "game_modes/menus/title_screen_mode.h"
#include "game_modes/menus/change_language_mode.h"
#include "io/input/input_handler.h"
#include "io/local/local_handler.h"
#include "logger/logger.h"
#include "memory/mem_mgmt.h"

#include <stdlib.h>
#include <time.h>

int init();
void shutdown();

int init() {
    tb_init();
    init_logger();
    init_input_handler();
    if (init_local_handler(LANGE_EN) != 0) return 1;
    if (init_title_screen() != 0) return 2;
    if (init_change_language() != 0) return 3;

    // Seed the random number generator with a combination of time, process ID, and stack variable address
    unsigned int seed = (unsigned int) time(NULL);// Use current time as seed
    seed ^= (unsigned int) getpid();              // XOR with process ID
    int stack_var;
    seed ^= (unsigned int) &stack_var;// XOR with the address of a stack variable
    srand(seed);                      // Seed the random number generator

    return 0;
}

int main(void) {
    const int exit_code = init();

    if (exit_code == 0) {
        memory_pool_t* pool = init_memory_pool(2 * STANDARD_MEMORY_POOL_SIZE);// 16 MB
        //the memory pool doesn't need to be checked here, as it will be checked in start_game_loop()

        DEBUG_LOG("Main", "Starting game loop");
        start_game_loop(pool);

        shutdown_memory_pool(pool);
    }
    shutdown();
    return exit_code;
}

void shutdown() {
    shutdown_change_language();
    shutdown_title_screen();
    shutdown_local_handler();
    shutdown_input_handler();
    shutdown_logger();
    tb_shutdown();
}
