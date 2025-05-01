#include "io/input/input_handler.h"
#include "logger/logger.h"
#include "termbox2/termbox2.h"

void init();
void shutdown();

void init() {
    tb_init();
    init_logger();
    init_input_handler();
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
