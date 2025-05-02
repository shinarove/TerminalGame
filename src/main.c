#include "../termbox2/termbox2.h"
#include "io/input/input_handler.h"
#include "logger/logger.h"
#include "memory/mem_mgmt.h"
#include "game.h"

#include <stdlib.h>
#include <time.h>

void init();
void shutdown();

void init() {
    tb_init();
    init_logger();
    init_input_handler();

    // Seed the random number generator with a combination of time, process ID, and stack variable address
    unsigned int seed = (unsigned int) time(NULL);// Use current time as seed
    seed ^= (unsigned int) getpid();              // XOR with process ID
    int stack_var;
    seed ^= (unsigned int) &stack_var;// XOR with the address of a stack variable
    srand(seed);                      // Seed the random number generator
}

int main(void) {
    init();
    memory_pool_t* pool = init_memory_pool(STANDARD_MEMORY_POOL_SIZE);
    //the memory pool doesn't need to be checked here, as it will be checked in start_game_loop()

    log_msg(FINE, "Main", "This is a test log message");
    start_game_loop(pool);

    shutdown_memory_pool(pool);
    shutdown();
    return 0;
}

void shutdown() {
    shutdown_input_handler();
    shutdown_logger();
    tb_shutdown();
}
