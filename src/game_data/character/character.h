#ifndef CHARACTER_H
#define CHARACTER_H

#include "../ability/ability.h"
#include "../inventory/inventory.h"
#include "stats.h"

typedef struct character {
    int id;//currently not in use
    int current_exp;
    int needed_exp;
    int level;
    int has_map_key;// states if the character has the map key, 0 = no, 1 = yes
    int unspent_attr_p;
    int unspent_res_p;
    int max_carry_weight;// the maximum weight the character can carry

    resources_t base_resources;     // base = character without gear
    resources_t max_resources;      // max = base + buffs from gear
    resources_t current_resources;  // current usable resources, current <= max
    attributes_t base_attributes;   // base = character without gear
    attributes_t max_attributes;    // max = base + buffs from gear
    attributes_t current_attributes;// current = max +- temporary buffs / debuffs

    char* name;
    ability_array_t* abilities;
    inventory_t* inventory;// the character's inventory
} character_t;

/**
 * Creates an empty character with default attributes, resources, and inventory values.
 * The character's memory is allocated from the global memory pool.
 *
 * @return A pointer to the newly created character structure, or NULL if memory allocation fails.
 */
character_t* create_empty_character(void);

/**
 * Creates a base character with default attributes, resources, and inventory values.
 * Allocates memory for the character structure from the global memory pool and
 * initializes all properties to their default or provided values.
 *
 * @param id The unique identifier for the character.
 * @param name The name of the character. Must be a non-null string.
 * @return A pointer to the newly created character structure, or NULL if memory allocation fails
 *         or if the provided name is NULL.
 */
character_t* create_base_character(int id, const char* name);

/**
 * Frees all memory associated with the given character, including its name, abilities,
 * inventory, and any dynamically allocated resources or structures within it.
 * Once this function is called, the pointer to the character should no longer be used.
 *
 * @param character A pointer to the character structure to be destroyed. Must not be NULL.
 */
void destroy_character(character_t* character);

/**
 * Adds resource values (health, stamina, and mana) to a character, ensuring that
 * current resources do not exceed their respective maximum values.
 *
 * @param character A pointer to the character whose resources are to be modified. Must not be NULL.
 * @param health The amount of health to add to the character. Values are clamped so that the total does not exceed max health.
 * @param stamina The amount of stamina to add to the character. Values are clamped so that the total does not exceed max stamina.
 * @param mana The amount of mana to add to the character. Values are clamped so that the total does not exceed max mana.
 */
void add_resources_c(character_t* character, int health, int stamina, int mana);

/**
 * Resets the resources of the specified character to their maximum values.
 *
 * @param character A pointer to the character whose resources are to be reset. Must not be NULL.
 */
void reset_resources_c(character_t* character);

/**
 * Resets the health of the specified character to its maximum value.
 *
 * @param character A pointer to the character whose health needs to be reset. Must not be NULL.
 */
void reset_health_c(character_t* character);

/**
 * Resets the stamina of the specified character to its maximum value.
 *
 * @param character A pointer to the character whose stamina is to be reset. Must not be NULL.
 */
void reset_stamina_c(character_t* character);

/**
 * Resets the mana of the specified character to its maximum value.
 *
 * @param character A pointer to the character whose mana is to be reset. Must not be NULL.
 */
void reset_mana_c(character_t* character);

/**
 * Checks if the character's current experience is enough to meet or exceed
 * the required experience for the next level.
 *
 * @param character A pointer to the character whose experience is being checked. Must not be NULL.
 * @return 1 if the character's current experience is greater than or equal to the required experience, 0 otherwise.
 */
int check_exp_c(const character_t* character);

/**
 * Levels up the specified character, increasing its level and updating its resources
 * and attributes based on predetermined calculations. One primary attribute can be
 * chosen for an additional increase upon leveling up.
 *
 * @param character A pointer to the character to level up. Must not be NULL.
 *                  If the character is already at the maximum level (20), no changes
 *                  will be made, and a warning message will be logged.
 * @param attr_to_increase The identifier of the attribute to increase upon leveling up.
 *                         Acceptable values are: STRENGTH, INTELLIGENCE, AGILITY,
 *                         ENDURANCE, and LUCK. If an invalid attribute identifier
 *                         is provided, an error message will be logged and no
 *                         attribute is increased.
 */
void lvl_up_c(character_t* character, attr_id_t attr_to_increase);

int add_gear_c(character_t* character, gear_id_t gear_id);

int remove_gear_c(const character_t* character, gear_id_t gear_id);

gear_t* get_gear_by_id_c(const character_t* character, gear_id_t gear_id);

/**
 * Adds a specified ability to a character's ability array. If the ability array is not initialized,
 * it will be created with a default size of 5.
 * If the ability already exists in the array, it will not be added again.
 *
 * @param character The character to which the ability is to be added. Must not be NULL.
 * @param ability_id The ID of the ability to be added to the character's abilities.
 * @return 0 on success, non-zero on failure
 */
int add_ability_c(character_t* character, ability_id_t ability_id);

/**
 * Removes a specified ability from a character's ability array.
 * This function checks if the character pointer is valid and delegates
 * the removal process to the underlying ability array management function.
 *
 * @param character A pointer to the character structure from which the ability is to be removed.
 *                  Must not be NULL.
 * @param ability_id The ID of the ability to be removed from the character's abilities.
 *                   Must represent a valid ability ID.
 * @return 0 on success, non-zero on failure
 */
int remove_ability_c(const character_t* character, ability_id_t ability_id);

/**
 * Retrieves a pointer to an ability with the specified ID from the character's list of abilities.
 *
 * @param character A pointer to the character from which the ability is to be retrieved. Must not be NULL.
 * @param ability_id The unique identifier of the ability to find.
 * @return A pointer to the matching ability if found, or NULL if no matching ability exists or the character is NULL.
 */
ability_t* get_ability_by_id_c(const character_t* character, ability_id_t ability_id);

/**
 * Retrieves an ability from the character's ability list based on the provided index.
 * The function validates the input parameters to ensure the character and its abilities
 * are properly initialized and the index is within a valid range.
 *
 * @param character A pointer to the character structure containing the ability list.
 *                  Must not be NULL, and the ability array must be properly initialized.
 * @param index The index of the ability to retrieve. Must be within the valid bounds of the ability array.
 * @return A pointer to the ability structure at the specified index, or NULL if the character,
 *         ability list, or index is invalid.
 */
ability_t* get_ability_by_index_c(const character_t* character, int index);

#endif//CHARACTER_H
