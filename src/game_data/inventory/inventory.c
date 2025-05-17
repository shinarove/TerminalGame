#include "inventory.h"

#include "../../game.h"
#include "../../logger/logger.h"

int find_gear_by_id(const inventory_t* inventory, gear_id_t gear_id);

gear_slot_t parse_gear_type(gear_type_t gear_type);

inventory_t* create_inventory(const int pre_length) {
    RETURN_WHEN_TRUE(pre_length < 0, NULL, "Inventory", "In `create_empty_inventory` allocated_space is negative")
    inventory_t* inventory = memory_pool_alloc(global_memory_pool, sizeof(inventory_t));
    RETURN_WHEN_NULL(inventory, NULL, "Inventory", "In `create_empty_inventory` failed to allocate memory for inventory")

    if (pre_length == 0) {
        inventory->gears = NULL;
    } else {
        inventory->gears = (gear_t**) memory_pool_alloc(global_memory_pool, sizeof(gear_t*) * pre_length);
        RETURN_WHEN_NULL(inventory->gears, NULL, "Inventory", "In `create_empty_inventory` failed to allocate memory for gears")

        for (int i = 0; i < pre_length; i++) {
            inventory->gears[i] = NULL;
        }
        for (int i = 0; i < MAX_GEAR_SLOTS; i++) {
            inventory->equipped[i] = NULL;
        }
    }

    inventory->gear_count = 0;
    inventory->allocated_space = pre_length;
    inventory->total_resource_bonus.health = 0;
    inventory->total_resource_bonus.stamina = 0;
    inventory->total_resource_bonus.mana = 0;
    inventory->total_attribute_bonus.strength = 0;
    inventory->total_attribute_bonus.intelligence = 0;
    inventory->total_attribute_bonus.agility = 0;
    inventory->total_attribute_bonus.constitution = 0;
    inventory->total_attribute_bonus.luck = 0;
    return inventory;
}

int add_gear_i(inventory_t* inventory, const gear_id_t gear_id) {
    RETURN_WHEN_NULL(inventory, 1, "Inventory", "In `add_gear_i` inventory is NULL")
    RETURN_WHEN_TRUE(gear_id < 0 || gear_id >= MAX_GEARS, 1,
                     "Inventory", "In `add_gear_i` given gear id is invalid: %d", gear_id)
    RETURN_WHEN_TRUE(inventory->gear_count > inventory->allocated_space, 1,
                     "Inventory", "In `add_gear_i` invalid state encountered,"
                                  "the gear count is greater than the allocated space: %d > %d",
                     inventory->gear_count, inventory->allocated_space)

    if (inventory->gears == NULL || inventory->allocated_space == 0) {
        // when the inventory is empty, allocate space for the gear with default size of 5
        inventory->gears = (gear_t**) memory_pool_alloc(global_memory_pool, sizeof(gear_t*) * 5);
        for (int i = 0; i < MAX_GEAR_SLOTS; i++) {
            inventory->gears[i] = NULL;
        }
        inventory->allocated_space = 5;
    } else if (inventory->gear_count == inventory->allocated_space) {
        // when the inventory is full, double the size
        inventory->allocated_space *= 2;
        inventory->gears = (gear_t**) memory_pool_realloc(global_memory_pool, inventory->gears,
                                                          sizeof(gear_t*) * inventory->allocated_space);
        RETURN_WHEN_NULL(inventory->gears, 1, "Inventory", "In `add_gear_i` failed to reallocate memory for gears")
    }
    // add the gear to the inventory
    inventory->gears[inventory->gear_count] = &get_gear_table()->gears[gear_id];
    inventory->gear_count++;

    return 0;
}

int remove_gear_i(inventory_t* inventory, const gear_id_t gear_id) {
    RETURN_WHEN_NULL(inventory, 1, "Inventory", "In `add_gear_i` inventory is NULL")
    RETURN_WHEN_TRUE(gear_id < 0 || gear_id >= MAX_GEARS, 1,
                     "Inventory", "In `add_gear_i` given gear id is invalid: %d", gear_id)

    // find the gear in the inventory
    const int index = find_gear_by_id(inventory, gear_id);

    if (index != -1) {
        // remove the gear from the inventory
        for (int i = index; i < inventory->gear_count - 1; i++) {
            inventory->gears[i] = inventory->gears[i + 1];
        }
        inventory->gears[inventory->gear_count - 1] = NULL;
        inventory->gear_count--;
        return 0;
    }
    return 1;// gear not found
}

int equip_gear_i(inventory_t* inventory, const gear_id_t gear_id, const gear_slot_t target_slot) {
    RETURN_WHEN_NULL(inventory, 1, "Inventory", "In `equip_gear_i` inventory is NULL")
    if (inventory->gear_count == 0) return 1;// no gears to equip in the inventory
    // find the gear in the inventory
    const int index = find_gear_by_id(inventory, gear_id);
    if (index == -1) return 1;// gear isn't found in the inventory

    // check if the gear matches with the slot
    gear_t* gear_to_equip = inventory->gears[index];
    if (gear_slot_type_mapping[target_slot].type != gear_to_equip->gear_type) {
        log_msg(WARNING, "Inventory", "In `equip_gear_i` gear type %d does not match the slot nr %d",
                gear_to_equip->gear_type, target_slot);
        return 1;
    }

    // ensure the correct inventory state
    switch (target_slot) {
        case HEAD_SLOT:
        case BODY_SLOT:
        case LEG_SLOT:
        case HAND_SLOT:
        case RING_LEFT_SLOT:
        case RING_RIGHT_SLOT:
        case AMULET_SLOT:
            unequip_gear_i(inventory, target_slot);
            break;
        case MAIN_HAND_SLOT:
            if (unequip_gear_i(inventory, MAIN_HAND_SLOT)) {
                // if nothing was unequipped, try to unequip the both hand slot
                unequip_gear_i(inventory, BOTH_HAND_SLOT);
            }
            break;
        case OFF_HAND_SLOT:
            if (unequip_gear_i(inventory, OFF_HAND_SLOT)) {
                // if nothing was unequipped, try to unequip the both hand slot
                unequip_gear_i(inventory, BOTH_HAND_SLOT);
            }
            break;
        case BOTH_HAND_SLOT:// unequip the items in the main-hand and off-hand slots
            if (unequip_gear_i(inventory, BOTH_HAND_SLOT)) {
                // if nothing was unequipped, try to unequip the main-hand and off-hand slots
                unequip_gear_i(inventory, MAIN_HAND_SLOT);
                unequip_gear_i(inventory, OFF_HAND_SLOT);
            }
            break;
        default:
            log_msg(WARNING, "Inventory", "In `equip_gear_i` invalid slot type %d", target_slot);
            return 1;
    }
    // equip the gear
    inventory->equipped[target_slot] = gear_to_equip;

    // add ressource & attribute bonuses in total
    inventory->total_resource_bonus.health += gear_to_equip->resource_bonus.health;
    inventory->total_resource_bonus.stamina += gear_to_equip->resource_bonus.stamina;
    inventory->total_resource_bonus.mana += gear_to_equip->resource_bonus.mana;
    inventory->total_attribute_bonus.strength += gear_to_equip->attribute_bonus.strength;
    inventory->total_attribute_bonus.intelligence += gear_to_equip->attribute_bonus.intelligence;
    inventory->total_attribute_bonus.agility += gear_to_equip->attribute_bonus.agility;
    inventory->total_attribute_bonus.constitution += gear_to_equip->attribute_bonus.constitution;
    inventory->total_attribute_bonus.luck += gear_to_equip->attribute_bonus.luck;

    return 0;
}

int unequip_gear_i(inventory_t* inventory, const gear_slot_t target_slot) {
    RETURN_WHEN_NULL(inventory, 1, "Inventory", "In `unequip_gear_i` inventory is NULL")

    if (inventory->equipped[target_slot] == NULL) return 1;// no gear to unequip in the inventory

    const gear_t* gear_to_unequip = inventory->equipped[target_slot];
    inventory->equipped[target_slot] = NULL;

    // remove ressource & attribute bonuses in total
    inventory->total_resource_bonus.health -= gear_to_unequip->resource_bonus.health;
    inventory->total_resource_bonus.stamina -= gear_to_unequip->resource_bonus.stamina;
    inventory->total_resource_bonus.mana -= gear_to_unequip->resource_bonus.mana;
    inventory->total_attribute_bonus.strength -= gear_to_unequip->attribute_bonus.strength;
    inventory->total_attribute_bonus.intelligence -= gear_to_unequip->attribute_bonus.intelligence;
    inventory->total_attribute_bonus.agility -= gear_to_unequip->attribute_bonus.agility;
    inventory->total_attribute_bonus.constitution -= gear_to_unequip->attribute_bonus.constitution;
    inventory->total_attribute_bonus.luck -= gear_to_unequip->attribute_bonus.luck;

    return 0;
}

void destroy_inventory(inventory_t* inventory) {
    if (inventory == NULL) return;

    if (inventory->gears != NULL) {
        memory_pool_free(global_memory_pool, inventory->gears);
    }

    memory_pool_free(global_memory_pool, inventory);
}

int find_gear_by_id(const inventory_t* inventory, const gear_id_t gear_id) {
    // find the gear in the inventory
    int index = -1;
    for (int i = 0; i < inventory->gear_count; i++) {
        if (inventory->gears[i]->id == gear_id) {
            index = i;
            break;
        }
    }
    return index;
}
