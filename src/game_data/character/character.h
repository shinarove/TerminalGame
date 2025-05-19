#ifndef CHARACTER_H
#define CHARACTER_H

#include "../ability/ability.h"
#include "../inventory/inventory.h"
#include "enemy_id.h"
#include "stats.h"

typedef struct character {
    int id;
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

    unsigned int health_u_flag : 1; // when either the base, max or current health was changed
    unsigned int stamina_u_flag : 1; // when either the base, max or current stamina was changed
    unsigned int mana_u_flag : 1; // when either the base, max or current mana was changed
    unsigned int strength_u_flag : 1; // when either the base, max or current strength was changed
    unsigned int intelligence_u_flag : 1; // when either the base, max or current intelligence was changed
    unsigned int agility_u_flag : 1; // when either the base, max or current agility was changed
    unsigned int constitution_u_flag : 1; // when either the base, max or current constitution was changed
    unsigned int luck_u_flag : 1; // when either the base, max or current luck was changed
} character_t;

static const struct {
    int id;
    ability_id_t basic_ability_id;// when no weapons are equipped, use these abilities
} character_base_ability[] = {
        {0, PUNCH},
        {GOBLIN, CLAWS}};

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

/**
 * Adds a gear item to the character's inventory if possible.
 * Automatically creates an inventory for the character if none exists.
 * Ensures that the same gear is not added twice and checks if the inventory is full.
 *
 * @param character A pointer to the character structure to which the gear will be added.
 *                  Must not be NULL.
 * @param gear_id The ID of the gear to be added to the character's inventory.
 *
 * @return 0 if the gear was successfully added,
 *         1 if the gear could not be added (e.g., inventory is full or gear already exists),
 *         or other values for error scenarios.
 */
int add_gear_c(character_t* character, gear_id_t gear_id);

/**
 * Removes a specific piece of gear from the specified character's inventory and equipment.
 * Ensures the character's inventory and attributes are updated after gear removal.
 *
 * @param character A pointer to the character structure from which the gear will be removed. Must not be NULL.
 * @param gear_id The unique identifier of the gear to be removed.
 * @return An integer indicating the success or failure of the removal process.
 *         Returns 1 if the character is NULL or the gear removal fails, otherwise 0.
 */
int remove_gear_c(const character_t* character, gear_id_t gear_id);

/**
 * Equips a specific gear item to a designated gear slot on the given character.
 * If the target slot or related slots are already occupied, the previously equipped gear
 * will be unequipped before equipping the new item. After equipping, any bonuses or
 * abilities associated with the gear item will be applied to the character.
 *
 * @param character A pointer to the character structure on which the gear will be equipped.
 *                  Must not be NULL.
 * @param gear_id The ID of the gear item to be equipped. Must correspond to a valid gear ID.
 * @param target_slot The gear slot where the item will be equipped. Must be within the
 *                    valid range of gear slots.
 * @return 0 if the gear is successfully equipped, or a non-zero value if an error occurs
 *         (e.g., invalid input or failure to equip the gear).
 */
int equip_gear_c(character_t* character, gear_id_t gear_id, gear_slot_t target_slot);

/**
 * Unequips the gear from a specific slot in the character's inventory.
 * Removes all abilities associated with the gear being unequipped and recalculates
 * the character's attributes and resources. If unequipping results in no weapon being equipped
 * in the main or both-hand slot, a base ability is added to the character.
 *
 * @param character A pointer to the character structure from which the gear will be unequipped.
 *                  Must not be NULL.
 * @param target_slot The gear slot from which the gear is to be unequipped. Must be a valid
 *                    gear slot defined in the gear_slot_t enumeration.
 * @return An integer indicating the success of the unequip operation:
 *         - 0 if successful.
 *         - 1 if the provided target slot is invalid, the character pointer is NULL, or
 *           no gear is equipped in the specified slot.
 */
int unequip_gear_c(character_t* character, gear_slot_t target_slot);

/**
 * Retrieves a gear from the character's inventory by its ID.
 * Searches through the character's inventory to find and return a pointer
 * to the gear structure matching the specified gear ID. If the gear does not exist
 * in the inventory or if the character or inventory is null, the method will return NULL.
 *
 * @param character A pointer to the character whose inventory will be searched.
 *                  Must not be NULL.
 * @param gear_id   The ID of the gear to be located in the character's inventory.
 *
 * @return A pointer to the gear structure if found, or NULL if the gear is not present,
 *         if the character or inventory is NULL, or if the inventory is not initialized.
 */
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

void reset_update_flags_c(character_t* character);

#endif//CHARACTER_H
