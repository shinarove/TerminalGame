#include "game.h"

#include "game_data/character/enemy_generator.h"
#include "game_data/map/map_generator.h"
#include "game_data/map/map_revealer.h"
#include "game_modes/character/character_creation_mode.h"
#include "game_modes/character/lvl_up_mode.h"
#include "game_modes/combat/combat_mode.h"
#include "game_modes/map/map_mode.h"
#include "game_modes/menus/language_menu_mode.h"
#include "game_modes/menus/load_game_mode.h"
#include "game_modes/menus/main_menu_mode.h"
#include "game_modes/menus/save_game_mode.h"
#include "game_modes/menus/title_screen_mode.h"
#include "io/input/input_handler.h"
#include "logger/logger.h"

#define FRAMES_PER_SECONDS 20.0

#define MAX_MAP_COUNT 5
#define MAP_HEIGHT 19
#define MAP_WIDTH 39
#define ENEMY_COUNT 4

memory_pool_t* global_memory_pool = NULL;

void start_game_loop(const memory_pool_t* used_pool) {
    //allocate maps in the memory pool, with max number of maps
    map_t** maps = memory_pool_alloc(used_pool, MAX_MAP_COUNT * sizeof(map_t*));
    for (int i = 0; i < MAX_MAP_COUNT; i++) {
        maps[i] = NULL;
    }

    bool running = true;
    state_t current = TITLE_SCREEN;
    state_t return_to = TITLE_SCREEN;

    game_state_t game_state = {
            .max_floors = 0,       // on which floor the player is 1 - 5, 0 - no floor
            .active_map_index = -1,//-1 means no map is active
            .maps = maps,
            .player = create_empty_character()};
    character_t* enemy = NULL;

    while (running) {
        usleep((unsigned int) (1.0 / FRAMES_PER_SECONDS * 1000000.0));// wait for 1 frame
        const input_t input = get_next_input();

        switch (current) {
            case TITLE_SCREEN:
                current = update_title_screen(input);
                if (current == LANGUAGE_MODE) return_to = TITLE_SCREEN;
                if (current == LOAD_GAME) {
                    return_to = TITLE_SCREEN;
                    current = prepare_load_game_mode(used_pool, &game_state);
                }
                break;
            case GENERATE_MAP: {
                if (game_state.max_floors == MAX_MAP_COUNT) break;// last floor reached, can't generate more maps

                game_state.active_map_index += 1;
                game_state.max_floors += 1;
                maps[game_state.active_map_index] = memory_pool_alloc(used_pool, sizeof(map_t));
                if (maps[game_state.active_map_index] == NULL) {
                    log_msg(ERROR, "Game", "Failed to allocate memory for map");
                    running = false;
                }
                //initialize the map
                maps[game_state.active_map_index]->floor_nr = game_state.max_floors;
                maps[game_state.active_map_index]->width = MAP_WIDTH;
                maps[game_state.active_map_index]->height = MAP_HEIGHT;
                maps[game_state.active_map_index]->enemy_count = ENEMY_COUNT;

                if (generate_map(used_pool, maps[game_state.active_map_index],
                                 game_state.max_floors != MAX_MAP_COUNT) != 0) {
                    log_msg(ERROR, "Game", "Failed to generate map");
                    running = false;
                } else {
                    // reveal the map around the player starting position
                    reveal_map(maps[game_state.active_map_index], 3);
                    current = MAP_MODE;
                }
                break;
            }
            case GENERATE_ENEMY:
                enemy = generate_goblin(game_state.player->level);
                if (enemy == NULL) {
                    log_msg(ERROR, "Game", "Failed to generate enemy");
                    running = false;
                } else {
                    current = prepare_combat_mode(game_state.player, enemy);
                }
                break;
            case CHARACTER_CREATION:
                current = update_character_creation(input, game_state.player);
                break;
            case RESTART_GAME:
                // destroy current player & creating empty character
                destroy_character(game_state.player);
                game_state.player = create_empty_character();

                // free all the previously created maps
                for (int i = 0; i < game_state.max_floors; i++) {
                    destroy_map(used_pool, maps[i]);
                }
                game_state.active_map_index = -1;
                game_state.max_floors = 0;// reset the max floor

                // change to character creation mode
                current = CHARACTER_CREATION;
                break;
            case SAVE_GAME:
                current = update_save_game_mode(input);
                break;
            case LOAD_GAME:
                current = update_load_game_mode(input, return_to);
                if (current == TITLE_SCREEN && game_state.player == NULL) {// loading has failed
                    log_msg(WARNING, "Game", "Loading failed, resetting game state to default values.");
                    game_state.max_floors = 0;
                    game_state.active_map_index = -1;
                    game_state.player = create_empty_character();
                    // maps already set to NULL, after the loading attempt
                }
                break;
            case MAP_MODE:
                current = update_map_mode(input, maps[game_state.active_map_index], game_state.player);
                break;
            case ENTER_NEXT_FLOOR:
                if (game_state.active_map_index + 1 == game_state.max_floors) {
                    // the player is on the newest generated floor, generate a new map
                    current = GENERATE_MAP;
                } else if (game_state.active_map_index + 1 < game_state.max_floors) {
                    // the player is on an older floor, go to the next floor
                    game_state.active_map_index += 1;
                    current = MAP_MODE;
                    maps[game_state.active_map_index]->player_pos = maps[game_state.active_map_index]->entry_pos;
                } else {
                    log_msg(ERROR, "Game",
                            "Invalid map index: %d", game_state.active_map_index);
                    current = EXIT_GAME;
                }
                break;
            case ENTER_PREV_FLOOR:
                if (game_state.active_map_index == 0) {
                    // revert the player position
                    maps[game_state.active_map_index]->player_pos = maps[game_state.active_map_index]->entry_pos;
                    current = MAP_MODE;
                } else if (game_state.active_map_index > 0) {
                    // go to the previous floor
                    game_state.active_map_index -= 1;
                    current = MAP_MODE;
                    maps[game_state.active_map_index]->player_pos = maps[game_state.active_map_index]->exit_pos;
                } else {
                    log_msg(ERROR, "Game", "Invalid map index: %d",
                            game_state.active_map_index);
                    current = EXIT_GAME;
                }
                break;
            case COMBAT_MODE:
                current = update_combat_mode(input, game_state.player, enemy);
                if (current != COMBAT_MODE) {
                    // combat mode has ended free / destroy the resources
                    destroy_character(enemy);
                    enemy = NULL;

                    if (check_exp_c(game_state.player)) {
                        current = prepare_lvl_up_mode(game_state.player);
                    }
                }
                break;
            case LVL_UP_MODE:
                current = update_lvl_up_mode(input, game_state.player);
                break;
            case INVENTORY_MODE:
            case CHARACTER_MODE:
            case MAIN_MENU:
                current = update_main_menu(input);
                if (current == LANGUAGE_MODE) return_to = MAIN_MENU;
                if (current == SAVE_GAME) {
                    current = prepare_save_game_mode(&game_state);
                }
                if (current == LOAD_GAME) {
                    return_to = MAIN_MENU;
                    current = prepare_load_game_mode(used_pool, &game_state);
                }
                break;
            case LANGUAGE_MODE:
                current = update_change_language(input, return_to);
                break;
            case GAME_OVER:
                log_msg(INFO, "Game",
                        "Game Over isn't completed: %d, now exiting game.", current);
            case EXIT_GAME:
                running = false;
                break;
        }
    }

    destroy_character(game_state.player);
    for (int i = 0; i < game_state.max_floors; i++) {
        if (maps[i] != NULL) memory_pool_free(used_pool, maps[i]);
    }
    memory_pool_free(used_pool, maps);
    memory_pool_free(used_pool, game_state.player);
}
