#include "game.h"

#include "game_data/character/enemy_generator.h"
#include "game_data/map/map_generator.h"
#include "game_data/map/map_revealer.h"
#include "game_modes/character/character_creation_mode.h"
#include "game_modes/character/lvl_up_mode.h"
#include "game_modes/combat/combat_mode.h"
#include "game_modes/map/map_mode.h"
#include "game_modes/menus/language_menu_mode.h"
#include "game_modes/menus/main_menu_mode.h"
#include "game_modes/menus/title_screen_mode.h"
#include "io/input/input_handler.h"
#include "logger/logger.h"

#define FRAMES_PER_SECONDS 20.0

#define MAX_MAP_COUNT 5
#define MAP_HEIGHT 19
#define MAP_WIDTH 39
#define ENEMY_COUNT 4

void start_game_loop(const memory_pool_t* used_pool) {
    //allocate maps in the memory pool, with max number of maps
    map_t** maps = memory_pool_alloc(used_pool, MAX_MAP_COUNT * sizeof(map_t*));

    character_t* player = create_empty_character(used_pool);
    character_t* enemy = NULL;

    bool running = true;
    state_t current = TITLE_SCREEN;
    state_t return_to = TITLE_SCREEN;
    int active_map_index = -1;//-1 means no map is active
    int max_floor = 0;    // on which floor the player is 1 - 5, 0 - no floor

    while (running) {
        usleep((unsigned int) (1.0 / FRAMES_PER_SECONDS * 1000000.0));// wait for 1 frame
        const input_t input = get_next_input();

        switch (current) {
            case TITLE_SCREEN:
                current = update_title_screen(input);
                if (current == LANGUAGE_MODE) return_to = TITLE_SCREEN;
                break;
            case GENERATE_MAP: {
                if (max_floor == MAX_MAP_COUNT) break; // last floor reached, can't generate more maps

                active_map_index += 1;
                max_floor += 1;
                maps[active_map_index] = memory_pool_alloc(used_pool, sizeof(map_t));
                if (maps[active_map_index] == NULL) {
                    log_msg(ERROR, "Game", "Failed to allocate memory for map");
                    running = false;
                }
                //initialize the map
                maps[active_map_index]->floor_nr = max_floor;
                maps[active_map_index]->width = MAP_WIDTH;
                maps[active_map_index]->height = MAP_HEIGHT;
                maps[active_map_index]->enemy_count = ENEMY_COUNT;

                if (generate_map(used_pool, maps[active_map_index], max_floor != MAX_MAP_COUNT) != 0) {
                    log_msg(ERROR, "Game", "Failed to generate map");
                    running = false;
                } else {
                    // reveal the map around the player starting position
                    reveal_map(maps[active_map_index], 3);
                    current = MAP_MODE;
                }
                break;
            }
            case GENERATE_ENEMY:
                enemy = generate_goblin(used_pool, player->level);
                if (enemy == NULL) {
                    log_msg(ERROR, "Game", "Failed to generate enemy");
                    running = false;
                } else {
                    current = prepare_combat_mode(player, enemy);
                }
                break;
            case CHARACTER_CREATION:
                current = update_character_creation(input, player);
                break;
            case RESTART_GAME:
                // destroy current player & creating empty character
                destroy_character(used_pool, player);
                player = create_empty_character(used_pool);

                // free all the previously created maps
                for (int i = 0; i < max_floor; i++) {
                    if (maps[i] != NULL) memory_pool_free(used_pool, maps[i]);
                }
                active_map_index = -1;
                max_floor = 1; // reset the max floor

                // change to character creation mode
                current = CHARACTER_CREATION;
                break;
            case MAP_MODE:
                current = update_map_mode(input, maps[active_map_index], player);
                break;
            case ENTER_NEXT_FLOOR:
                if (active_map_index + 1 == max_floor) {
                    // the player is on the newest generated floor, generate a new map
                    DEBUG_LOG("Game", "Entering next floor, generating new map.");
                    current = GENERATE_MAP;
                } else if (active_map_index + 1 < max_floor) {
                    // the player is on an older floor, go to the next floor
                    DEBUG_LOG("Game", "Entering next floor, loading map %d.", active_map_index + 1);
                    active_map_index += 1;
                    current = MAP_MODE;
                    maps[active_map_index]->player_pos = maps[active_map_index]->entry_pos;
                } else {
                    log_msg(ERROR, "Game", "Invalid map index: %d", active_map_index);
                    current = EXIT_GAME;
                }
                break;
            case ENTER_PREV_FLOOR:
                if (active_map_index == 0) {
                    // revert the player position
                    maps[active_map_index]->player_pos = maps[active_map_index]->entry_pos;
                    current = MAP_MODE;
                } else if (active_map_index > 0) {
                    // go to the previous floor
                    active_map_index -= 1;
                    current = MAP_MODE;
                    maps[active_map_index]->player_pos = maps[active_map_index]->exit_pos;
                } else {
                    log_msg(ERROR, "Game", "Invalid map index: %d", active_map_index);
                    current = EXIT_GAME;
                }
                break;
            case COMBAT_MODE:
                current = update_combat_mode(input, player, enemy);
                if (current != COMBAT_MODE) {
                    // combat mode has ended free / destroy the resources
                    free_prepared_cm_resources();
                    destroy_character(used_pool, enemy);
                    enemy = NULL;

                    if (check_exp_c(player)) {
                        current = prepare_lvl_up_mode(player);
                    }
                }
                break;
            case LVL_UP_MODE:
                current = update_lvl_up_mode(input, player);
                if (current != LVL_UP_MODE) {
                    // level up mode has ended free the prepared resources
                    free_prepared_lum_resources();
                }
                break;
            case INVENTORY_MODE:
            case CHARACTER_MODE:
            case MAIN_MENU:
                current = update_main_menu(input);
                if (current == LANGUAGE_MODE) return_to = MAIN_MENU;
                break;
            case LANGUAGE_MODE:
                current = update_change_language(input, return_to);
                break;
            case GAME_OVER:
                log_msg(INFO, "Game", "Game Over isn't completed: %d, now exiting game.", current);
            case EXIT_GAME:
                running = false;
                break;
        }
    }

    destroy_character(used_pool, player);
    for (int i = 0; i < max_floor; i++) {
        if (maps[i] != NULL) memory_pool_free(used_pool, maps[i]);
    }
    memory_pool_free(used_pool, maps);
    memory_pool_free(used_pool, player);
}
