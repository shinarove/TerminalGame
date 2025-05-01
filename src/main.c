#include "io/input/input_handler.h"
#include "logger/logger.h"
#include "../termbox2/termbox2.h"

#include <time.h>
#include <stdlib.h>

void init();
void shutdown();

void init() {
    tb_init();
    init_logger();
    init_input_handler();

    // Seed the random number generator with a combination of time, process ID, and stack variable address
    unsigned int seed = (unsigned int) time(NULL); // Use current time as seed
    seed ^= (unsigned int) getpid(); // XOR with process ID
    int stack_var;
    seed ^= (unsigned int) &stack_var; // XOR with the address of a stack variable
    srand(seed); // Seed the random number generator
}

int main(void) {
    init();

    log_msg(FINE, "Main", "This is a test log message");

    shutdown();
    return 0;
}

void shutdown() {
    shutdown_input_handler();
    shutdown_logger();
    tb_shutdown();
}
