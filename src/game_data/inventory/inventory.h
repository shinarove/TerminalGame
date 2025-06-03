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

    ArrayList* gear_list; // the array list of gears, holds the gear pointers
    const gear_t* equipped[MAX_GEAR_SLOTS];// the equipped gears
    const Inventory_VTable* vtable;  // the virtual table for inventory operations
};

struct Inventory_VTable {
    int (*add_gear)(const Inventory* self, const gear_t* gear);
    int (*remove_gear)(const Inventory* self, const gear_t* gear);
    int (*equip_gear)(Inventory* self, const gear_t* gear);
    int (*unequip_gear)(Inventory* self, gear_slot_t target_slot);
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
