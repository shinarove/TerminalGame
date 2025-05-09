#ifndef COMBAT_MODE_H
#define COMBAT_MODE_H

#include "../../game.h"
#include "../../game_data/character/character.h"
#include "../../io/input/input_handler.h"

/**
 * Initializes the combat mode of the game.
 *
 * This function allocates memory for combat mode strings, initializes
 * the combat mode menus, and sets up the function to observe and update
 * local settings. It also ensures proper memory management by checking
 * for successful memory allocation.
 *
 * @return 0 on successful initialization, or an error code if memory allocation fails.
 *
 * @details
 * - Allocates memory for `combat_mode_strings` to hold text for combat UI elements.
 * - Sets up the main combat menu with predefined options like abilities and potions.
 * - Initializes ability and potion menus with default NULL options.
 * - Updates localized content by calling `update_combat_mode_local`.
 * - Observes for changes in localization settings with `observe_local`.
 */
int init_combat_mode(void);

/**
 * Prepares the combat mode by initializing UI elements and menus for the provided player and enemy characters.
 *
 * This function sets up player and enemy name-level strings, initializes the ability menu based on the player's abilities,
 * and prepares the UI for switching to combat mode. It ensures necessary resources are allocated
 * and verifies the integrity of the inputs.
 *
 * @param player The pointer to the player's character object. Must not be NULL.
 * @param enemy The pointer to the enemy's character object. Must not be NULL.
 *
 * @return The game state after successfully preparing the combat mode. Returns `COMBAT_MODE` for successful initialization or an error state (e.g., `EXIT_GAME`) if a NULL parameter, allocation failure, or inconsistency occurs.
 *
 * @details
 * - Frees previous `combat_mode_strings` and allocates new memory for UI strings.
 * - Constructs strings for displaying the enemy and player's name with their respective levels.
 * - Sets up an ability menu based on the player's available abilities. Ensures that the count matches the linked list size.
 * - Prepares an empty potion menu with default settings.
 * - Calls `update_combat_head` to finalize UI setup for combat.
 * - Uses macros like `RETURN_WHEN_NULL` and `RETURN_WHEN_TRUE` for input validation and error handling.
 */
state_t prepare_combat_mode(const character_t* player, const character_t* enemy);

/**
 * Updates the combat mode based on user input and current game state.
 *
 * This function handles the main logic for the combat mode, including processing
 * player inputs, displaying combat-related UI, and determining the next game state.
 * It ensures the validity of the combat-related components, such as the player,
 * enemy, and combat strings. If these components are not initialized, the function
 * transitions to the game exit state.
 *
 * @param input The user input command, represented as an `input_t` enum.
 * @param player A pointer to the player's character structure, which holds player stats and abilities.
 * @param enemy A pointer to the enemy's character structure, which holds enemy stats and abilities.
 *
 * @return The next game state, represented as a `state_t` enum, which determines how the game proceeds
 *         after this function is called. It typically returns `COMBAT_MODE` but can transition to other states.
 */
state_t update_combat_mode(input_t input, character_t* player, character_t* enemy);

/**
 * Frees resources prepared during combat mode initialization.
 *
 * This function ensures proper memory deallocation for dynamically allocated
 * combat-related resources, including pre-generated string data and menu options.
 * It also sets pointers to NULL to avoid dangling references.
 *
 * @details
 * - Iterates over the range of prepared combat mode strings (`ENEMY_NAME` to `MAX_COMBAT_MODE_STRINGS`),
 *   deallocating memory for non-NULL string entries in the `combat_mode_strings` array.
 * - Deallocates memory for the `options` field of both `combat_mode_ability_menu`
 *   and `combat_mode_potion_menu` if they are not NULL.
 * - Resets the `options` pointers for the ability menu and potion menu to NULL
 *   after freeing the memory to prevent reuse.
 */
void free_prepared_cm_resources();

/**
 * Shuts down and cleans up resources associated with combat mode.
 *
 * This function ensures proper deallocation of memory used in combat mode
 * by freeing all dynamically allocated combat mode strings, the ability menu options,
 * and the potion menu options.
 *
 * @details
 * - Frees each string in `combat_mode_strings` if allocated, then releases
 *   the memory for the entire string array.
 * - Cleans up resources for the `combat_mode_ability_menu` by freeing its options
 *   array if it is not NULL.
 * - Cleans up resources for the `combat_mode_potion_menu` by freeing its options
 *   array if it is not NULL.
 * - Ensures no memory leaks occur for resources allocated during the initialization
 *   and runtime of combat mode.
 */
void shutdown_combat_mode(void);

#endif//COMBAT_MODE_H
