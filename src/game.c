#include "game.h"

#include "game_data/character/enemy_generator.h"
#include "game_data/map/map_generator.h"
#include "game_data/map/map_revealer.h"
#include "game_modes/combat/combat_mode.h"
#include "game_modes/map/map_mode.h"
#include "game_modes/menus/language_menu_mode.h"
#include "game_modes/menus/main_menu_mode.h"
#include "game_modes/menus/title_screen_mode.h"
#include "io/input/input_handler.h"
#include "logger/logger.h"

#define FRAMES_PER_SECONDS 20

#define MAX_MAP_COUNT 16
#define MAP_HEIGHT 19
#define MAP_WIDTH 39
#define ENEMY_COUNT 4

void start_game_loop(const memory_pool_t* used_pool, character_t* player) {
    //allocate maps in the memory pool, with max number of maps
    map_t** maps = memory_pool_alloc(used_pool, MAX_MAP_COUNT * sizeof(map_t*));
    character_t* enemy = NULL;

    bool running = true;
    state_t current = TITLE_SCREEN;
    state_t return_to = TITLE_SCREEN;
    int active_map_index = -1;//-1 means no map is active
    int map_count = 0;        // number of maps that have been generated

    while (running) {
        usleep((unsigned int) (1.0 / FRAMES_PER_SECONDS * 1000000.0));// wait for 1 frame
        const input_t input = get_next_input();

        switch (current) {
            case TITLE_SCREEN:
                current = update_title_screen(input);
                if (current == LANGUAGE_MODE) return_to = TITLE_SCREEN;
                break;
            case GENERATE_MAP: {
                active_map_index = active_map_index == -1 ? 0 : (active_map_index + 1) % MAX_MAP_COUNT;
                map_count = map_count == MAX_MAP_COUNT ? MAX_MAP_COUNT : map_count + 1;
                maps[active_map_index] = memory_pool_alloc(used_pool, sizeof(map_t));
                if (maps[active_map_index] == NULL) {
                    log_msg(ERROR, "Game", "Failed to allocate memory for map");
                    running = false;
                }
                //initialize the map
                maps[active_map_index]->id = active_map_index;
                maps[active_map_index]->width = MAP_WIDTH;
                maps[active_map_index]->height = MAP_HEIGHT;
                maps[active_map_index]->enemy_count = ENEMY_COUNT;

                if (generate_map(used_pool, maps[active_map_index]) != 0) {
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
            case MAP_MODE:
                current = update_map_mode(input, maps[active_map_index], player);
                break;
            case COMBAT_MODE:
                current = update_combat_mode(input, player, enemy);
                if (current != COMBAT_MODE) {
                    // combat mode has ended free / destroy the resources
                    free_prepared_resources();
                    destroy_character(used_pool, enemy);
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

    for (int i = 0; i < map_count; i++) {
        memory_pool_free(used_pool, maps[i]);
    }
    memory_pool_free(used_pool, maps);
    memory_pool_free(used_pool, player);
}
