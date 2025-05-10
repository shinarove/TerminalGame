#ifndef SAVE_FILE_HANDLER_H
#define SAVE_FILE_HANDLER_H

#include "map/map.h"
#include "character/character.h"
#include "../memory/mem_mgmt.h"

typedef struct {
    int max_floors;
    int active_map_index;
    map_t** maps;
    character_t* player;
} game_state_t;

/**
 * Saves the current game state to a file with the specified name.
 *
 * @param save_name The name of the save file, which will be stored in the save_files directory.
 * @param game_state A pointer to the game_state_t structure containing all game-related data to be saved.
 * @return Returns 0 on successful save operation. Returns 1 if an error occurs, such as failing to open the file.
 */
int save_game_state(const char* save_name, const game_state_t* game_state);

/**
 * Loads a game state from a specified save file into the provided game state structure.
 *
 * @param save_name The name of the save file to load, located in the save_files directory.
 * @param pool A pointer to the memory pool used for allocating necessary structures during loading.
 * @param game_state A pointer to the game_state_t structure where the loaded game data will be stored.
 * @return Returns 0 on successful load operation. Returns 1 if an error occurs, such as failing to open the file or file corruption.
 * @note The pointer of pointers to the maps must be allocated and set to NULL in the game_state_t structure.
 */
int load_game_state(char* save_name, const memory_pool_t* pool, game_state_t* game_state);


#endif //SAVE_FILE_HANDLER_H
