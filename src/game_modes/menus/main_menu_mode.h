#ifndef MAIN_MENU_MODE_H
#define MAIN_MENU_MODE_H

#include "../../game.h"
#include "../../io/input/input_handler.h"

int init_main_menu(void);

state_t update_main_menu(input_t input);

void shutdown_main_menu(void);

#endif//MAIN_MENU_MODE_H
