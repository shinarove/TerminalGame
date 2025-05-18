#include "output.h"

#include "specific/character_output.h"

int init_output(void) {
    return init_character_output();
}

void shutdown_output(void) {
    shutdown_character_output();
}
