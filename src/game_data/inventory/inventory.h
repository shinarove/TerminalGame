#ifndef INVENTORY_H
#define INVENTORY_H

#include "../../cstd/collections/array_list.h"
#include "../item/gear.h"

typedef struct Inventory Inventory;
typedef struct Inventory_VTable Inventory_VTable;

typedef enum {
    HEAD_SLOT,
    BODY_SLOT,
    LEG_SLOT,
    HAND_SLOT,
    RING_SLOT,
    AMULET_SLOT,
    MAIN_HAND_SLOT,// the main hand slot
    OFF_HAND_SLOT, // the off hand slot
    MAX_GEAR_SLOTS
} gear_slot_t;

struct Inventory {
    resources_t total_resource_bonus;  // the total resource bonus from all equipped gears
    attributes_t total_attribute_bonus;// the total attribute bonus from all equipped gears

    ArrayList* gear_list;                  // the array list of gears, holds the gear pointers
    const gear_t* equipped[MAX_GEAR_SLOTS];// the equipped gears
    const Inventory_VTable* vtable;        // the virtual table for inventory operations
};

struct Inventory_VTable {
    /**
     * Adds a gear to the inventory's gear list.
     *
     * @param self Pointer to the inventory instance.
     * @param gear Pointer to the gear to be added.
     * @return 0 on success, -1 on error.
     */
    int (*add_gear)(const Inventory* self, const gear_t* gear);
    /**
     * Removes a gear from the inventory's gear list.
     *
     * @param self Pointer to the inventory instance.
     * @param gear Pointer to the gear to be removed.
     * @return 0 on success, 1 if the gear was not found, -1 on error.
     */
    int (*remove_gear)(const Inventory* self, const gear_t* gear);
    /**
     * Equips a gear in the inventory, un-equipping any existing gear in the target slot.
     * If the gear is two-handed, it will unequip any gear in both the main and off-hand slots.
     *
     * @param self Pointer to the inventory instance.
     * @param gear Pointer to the gear to be equipped.
     * @return 0 on success, 1 if the gear was not found in the inventory, -1 on error.
     */
    int (*equip_gear)(Inventory* self, const gear_t* gear);
    /**
     * Unequips the gear from a specific slot in the inventory.
     * If the gear is two-handed, it will unequip both the main and off-hand slots.
     *
     * @param self Pointer to the inventory instance.
     * @param target_slot The gear slot from which the gear will be unequipped.
     * @return 0 on success, 1 if no gear was unequipped in the specified slot, -1 on error.
     */
    int (*unequip_gear)(Inventory* self, gear_slot_t target_slot);

    gear_t* (*get_gear_at)(const Inventory* self, int index);
    /**
     * Searches the inventory equipped slots, if the gear is equipped.
     *
     * @param self Pointer to the inventory instance.
     * @param gear Pointer to the gear to be checked.
     * @return 1 if the gear is equipped, 0 if not equipped, -1 on error.
     */
    int (*is_gear_equipped)(const Inventory* self, const gear_t* gear);
};

/**
 * Creates a new inventory structure with an initial capacity for gears.
 *
 * @param initial_capacity An unsigned integer representing the initial capacity of the inventory.
 *
 * @return A pointer to the newly created inventory_t structure if successful,
 *         or NULL if memory allocation fails or if pre_length is negative.
 */
Inventory* create_inventory(unsigned int initial_capacity);

/**
 * Destroys the inventory structure, freeing all it's resources and memory.
 *
 * @param inventory The pointer to the inventory_t structure to be destroyed.
 */
void destroy_inventory(Inventory* inventory);


#endif//INVENTORY_H
