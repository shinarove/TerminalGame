#ifndef SAVE_FILE_HANDLER_H
#define SAVE_FILE_HANDLER_H

#include "../memory/mem_mgmt.h"
#include "character/character.h"
#include "map/map.h"

typedef enum {
    SLOT_1,
    SLOT_2,
    SLOT_3,
    SLOT_4,
    SLOT_5,
    MAX_SAVE_SLOTS
} save_slot_t;

typedef struct {
    int max_floors;
    int active_map_index;
    map_t** maps;
    Character* player;
} game_state_t;

typedef struct {
    char** dates;// dates of the save slots, when NULL the save slot is empty
    int length;  // number of save slots
} save_infos_t;

/**
 * Saves the current game state to a save file associated with the specified save slot.
 *
 * @param save_slot The save slot identifier in which the game state should be saved. Must be between SLOT_1 and SLOT_5.
 * @param game_state A pointer to the game_state_t structure containing all necessary game data to save, including
 *                   map data, player information, and other state data.
 * @return Returns 0 on successful save operation. Returns 1 if an error occurs, such as an invalid save slot, inability
 *         to open the save file, null map references, or memory write issues.
 * @note The function ensures proper checks for null pointers and invalid data during the save operation. It writes metadata
 *       (e.g., timestamps and character data), maps, tiles, and player-related information sequentially into the save file.
 */
int save_game_state(save_slot_t save_slot, const game_state_t* game_state);

/**
 * Loads a saved game state from a save file associated with the specified save slot.
 *
 * @param save_slot The save slot identifier from which the game state should be loaded. Must be between SLOT_1 and SLOT_5.
 * @param pool A pointer to a memory_pool_t instance utilized for memory allocations during the loading process.
 * @param game_state A pointer to the game_state_t structure where the loaded game data will be stored. The structure
 *                   must be pre-allocated and capable of storing all game data, including maps and player information.
 * @return Returns 0 if the game state is successfully loaded. Returns 1 if an error occurs, such as an invalid save slot,
 *         memory allocation failure, file read errors, or corrupted save file data.
 * @note The function ensures that all resources are allocated using the provided memory pool, and it performs
 *       comprehensive error checking, including validation of the save slot, file integrity, and memory allocation failures.
 *       It reads metadata (e.g., timestamps, player information), maps, tiles, and player-related data sequentially.
 */
int load_game_state(save_slot_t save_slot, const memory_pool_t* pool, game_state_t* game_state);

/**
 * Retrieves information about the save files for all available save slots.
 *
 * This method checks the existence of save files in each save slot and extracts their corresponding
 * timestamp data if available. The resulting structure contains an array of timestamp strings where
 * each index represents the corresponding save slot. Non-existent or empty save slots are marked with NULL.
 *
 * @return A `save_infos_t` structure containing:
 *         - An array of timestamp strings (`dates`) associated with each save slot. NULL if the slot is empty.
 *         - The number of save slots (`length`).
 *         If an error occurs (e.g., memory allocation issues, file reading errors),
 *         an empty `save_infos_t` structure with a NULL `dates` pointer and `length` set to 0 is returned.
 */
save_infos_t get_save_infos(void);

#endif//SAVE_FILE_HANDLER_H
