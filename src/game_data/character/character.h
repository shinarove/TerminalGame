#ifndef CHARACTER_H
#define CHARACTER_H

#include "../../cstd/collections/array_list.h"
#include "../inventory/inventory.h"
#include "stats.h"

typedef struct Character Character;
typedef struct Character_VTable Character_VTable;

struct Character {
    int id;
    int current_exp;
    int needed_exp;
    int level;
    int has_map_key;// states if the character has the map key, 0 = no, 1 = yes
    int unspent_attr_p;
    int unspent_res_p;
    int current_carry_weight;// the current weight the character is carrying
    int max_carry_weight;    // the maximum weight the character can carry

    resources_t base_resources;     // base = character without gear
    resources_t max_resources;      // max = base + buffs from gear
    resources_t current_resources;  // current usable resources, current <= max
    attributes_t base_attributes;   // base = character without gear
    attributes_t max_attributes;    // max = base + buffs from gear
    attributes_t current_attributes;// current = max +- temporary buffs / debuffs

    char* name;
    ArrayList* ability_list;
    Inventory* inventory;// the character's inventory

    const Character_VTable* vtable;
};

struct Character_VTable {
    void (*reset_health)(Character* self);
    void (*reset_stamina)(Character* self);
    void (*reset_mana)(Character* self);
    int (*xp_limit_reached)(const Character* self);
    void (*lvl_up)(Character* self, attr_id_t attr_to_increase);
    /**
     * Picks up a gear and adds it to the character's inventory.
     *
     * @param self Pointer to the character structure.
     * @param gear Pointer to the gear to be picked up.
     * @return 0 on success, 1 on inventory is full, -1 on error.
     */
    int (*pick_up_gear)(Character* self, const gear_t* gear);// TODO: change to pick_up_item, after potions are implemented
    /**
     * Drops a gear from the character's inventory.
     *
     * @param self Pointer to the character structure.
     * @param gear Pointer to the gear to be dropped.
     * @return 0 on success, 1 if the gear is not found, -1 on error.
     */
    int (*drop_gear)(Character* self, const gear_t* gear);// TODO: change to drop_item, after potions are implemented
    /**
     * Equips a gear in the character's inventory, un-equipping any existing gear in the target slot.
     * If the gear is two-handed, it will unequip any gear in both the main and off-hand slots.
     *
     * @param self Pointer to the character structure.
     * @param gear Pointer to the gear to be equipped.
     * @return 0 on success, 1 if the gear was not found in the inventory, -1 on error.
     */
    int (*equip_gear)(Character* self, const gear_t* gear);
    /**
     * Unequips the gear from a specific slot in the character's inventory.
     * Removes all abilities associated with the gear being unequipped and recalculates
     * the character's attributes and resources.
     *
     * @param self Pointer to the character structure
     * @param target_slot The gear slot from which the gear is to be unequipped.
     * @return 0 on success, 1 if no gear was unequipped in the specified slot, -1 on error.
     */
    int (*unequip_gear)(Character* self, gear_slot_t target_slot);
    /**
     * Gets the gear at a specific index from the character's inventory.
     *
     * @param self Pointer to the character structure.
     * @param index The index of the gear to be retrieved.
     * @return Pointer to the gear at the specified index, or NULL if the index is out of bounds or an error occurs.
     */
    gear_t* (*get_gear_at)(const Character* self, int index);
    /**
     * Checks if a specific gear is currently equipped in the character's inventory.
     *
     * @param self Pointer to the character structure.
     * @param gear Pointer to the gear to be checked.
     * @return 1 if the gear is equipped, 0 if not equipped, -1 on error.
     */
    int (*is_gear_equipped)(const Character* self, const gear_t* gear);
    /**
     * Adds an ability to the character's ability list.
     * If the ability already exists, it will not be added again.
     *
     * @param self Pointer to the character structure.
     * @param ability Pointer to the ability to be added.
     * @return 0 on success, 1 if the ability is already in the list, -1 on error.
     */
    int (*add_ability)(const Character* self, const ability_t* ability);
    /**
     * Removes an ability from the character's ability list.
     * If the ability does not exist, it will return 1.
     *
     * @param self Pointer to the character structure.
     * @param ability Pointer to the ability to be removed.
     * @return 0 on success, 1 if the ability was not found, -1 on error.
     */
    int (*remove_ability)(const Character* self, const ability_t* ability);
    /**
     * Retrieves an ability at a specific index from the character's ability list.
     *
     * @param self Pointer to the character structure.
     * @param index The index of the ability to be retrieved.
     * @return Pointer to the ability at the specified index, or NULL if the index is out of bounds or an error occurs.
     */
    ability_t* (*get_ability_at)(const Character* self, int index);
};

/**
 * Creates an empty character with default values.
 *
 * @return A pointer to the newly created character structure,
 *         or NULL if an error occurs during creation.
 */
Character* create_empty_character(void);

/**
 * Creates a base character id,  name and default values.
 *
 * @param id The unique identifier for the character.
 * @param name The name of the character. Must be a non-null string.
 * @return A pointer to the newly created character structure,
 *         or NULL if an error occurs during creation.
 */
Character* create_base_character(int id, const char* name);

/**
 * Destroys the character structure, freeing all its resources and memory.
 *
 * @param character A pointer to the character structure to be destroyed.
 */
void destroy_character(Character* character);

#endif//CHARACTER_H
