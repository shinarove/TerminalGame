#include "inventory.h"

#include "../../game.h"
#include "../../logger/logger.h"

int add_gear(const Inventory* inventory, const gear_t* gear);
int remove_gear(const Inventory* inventory, const gear_t* gear);
int equip_gear(Inventory* inventory, const gear_t* gear);
int unequip_gear(Inventory* inventory, gear_slot_t target_slot);
int is_gear_equipped(const Inventory* inventory, const gear_t* gear);

static const Inventory_VTable vtable_Inventory = {
    .add_gear = add_gear,
    .remove_gear = remove_gear,
    .equip_gear = equip_gear,
    .unequip_gear = unequip_gear,
    .is_gear_equipped = is_gear_equipped
};

Inventory* create_inventory(const unsigned int initial_capacity) {
    RETURN_WHEN_TRUE(initial_capacity < 0, NULL, "Inventory", "In `create_inventory` allocated_space is negative")
    Inventory* inventory = malloc(sizeof(Inventory));
    RETURN_WHEN_NULL(inventory, NULL, "Inventory", "In `create_inventory` failed to allocate memory for inventory")

    inventory->gear_list = create_array_list(sizeof(gear_t*), initial_capacity);
    RETURN_WHEN_NULL(inventory->gear_list, NULL, "Inventory", "In `create_inventory` failed to create gear list")

    for (int i = 0; i < MAX_GEAR_SLOTS; i++) {
        inventory->equipped[i] = NULL;
    }

    inventory->total_resource_bonus = {0};
    inventory->total_attribute_bonus = {0};
    inventory->vtable = &vtable_Inventory;
    return inventory;
}

void destroy_inventory(Inventory* inventory) {
    if (inventory == NULL) return;

    destroy_array_list(inventory->gear_list);
    free(inventory);
}

int add_gear(const Inventory* inventory, const gear_t* gear) {
    RETURN_WHEN_NULL(inventory, -1, "Inventory", "In `add_gear` inventory is NULL")
    RETURN_WHEN_NULL(gear, -1, "Inventory", "In `add_gear` given gear is NULL")

    // add the gear reference to the gear list
    return inventory->gear_list->vtable->list->add(inventory->gear_list, &gear);
}

int remove_gear(const Inventory* inventory, const gear_t* gear) {
    RETURN_WHEN_NULL(inventory, -1, "Inventory", "In `remove_gear` inventory is NULL")
    RETURN_WHEN_NULL(gear, -1, "Inventory", "In `remove_gear` given gear is NULL")

    const int rm_success = inventory->gear_list->vtable->list->remove(inventory->gear_list, &gear);
    if (rm_success == -1) {
        log_msg(WARNING, "Inventory", "In `remove_gear` an error occurred while"
                                      "removing gear with id %d from the inventory", gear->id);
    } else if (rm_success == 1) {
        log_msg(INFO, "Inventory", "In `remove_gear` gear with id %d not found"
                                      "in the inventory", gear->id);
    }
    return rm_success;
}

int equip_gear(Inventory* inventory, const gear_t* gear) {
    RETURN_WHEN_NULL(inventory, -1, "Inventory", "In `equip_gear` inventory is NULL")
    RETURN_WHEN_NULL(gear, -1, "Inventory", "In `equip_gear` given gear is NULL")

    const int found = inventory->gear_list->vtable->list->find(inventory->gear_list, &gear);
    if (found == -2) {
        log_msg(WARNING, "Inventory", "In `equip_gear` an error occurred while"
                                      "finding gear with id %d in the inventory", gear->id);
        return -1;
    }
    if (found == -1) {
        log_msg(WARNING, "Inventory", "In `equip_gear` gear with id %d not found"
                                      "in the inventory", gear->id);
        return 1;
    }

    // gear is in the inventory, but we first need to unequip the gear in the target slot
    int unequip_result;
    if (gear->gear_type == TWO_HANDED) {
        unequip_result = inventory->vtable->unequip_gear(inventory, MAIN_HAND);
    } else {
        unequip_result = inventory->vtable->unequip_gear(inventory, gear->gear_type);
    }
    if (unequip_result < 0) {
        return unequip_result;
    }

    // either nothing was unequipped or the unequip was successful
    if (gear->gear_type == TWO_HANDED) {
        inventory->equipped[OFF_HAND_SLOT] = gear;
        inventory->equipped[MAIN_HAND_SLOT] = gear;
    } else {
        inventory->equipped[gear->gear_type] = gear;
    }

    // add ressource & attribute bonuses in total
    inventory->total_resource_bonus.health += gear->resource_bonus.health;
    inventory->total_resource_bonus.stamina += gear->resource_bonus.stamina;
    inventory->total_resource_bonus.mana += gear->resource_bonus.mana;
    inventory->total_attribute_bonus.strength += gear->attribute_bonus.strength;
    inventory->total_attribute_bonus.intelligence += gear->attribute_bonus.intelligence;
    inventory->total_attribute_bonus.agility += gear->attribute_bonus.agility;
    inventory->total_attribute_bonus.constitution += gear->attribute_bonus.constitution;
    inventory->total_attribute_bonus.luck += gear->attribute_bonus.luck;
    return 0;
}

int unequip_gear(Inventory* inventory, const gear_slot_t target_slot) {
    RETURN_WHEN_NULL(inventory, -1, "Inventory", "In `unequip_gear_slot` inventory is NULL")
    if (target_slot >= MAX_GEAR_SLOTS) {
        log_msg(WARNING, "Inventory", "In `unequip_gear_slot` invalid slot type %d", target_slot);
        return -1;
    }

    if (inventory->equipped[target_slot] == NULL) return 1;// no gear to unequip in the inventory
    const gear_t* gear_to_unequip = inventory->equipped[target_slot];

    if (gear_to_unequip->gear_type == TWO_HANDED &&
        (target_slot == OFF_HAND_SLOT || target_slot == MAIN_HAND_SLOT)) {
        // special case for two-handed weapons
        inventory->equipped[OFF_HAND_SLOT] = NULL;
        inventory->equipped[MAIN_HAND_SLOT] = NULL;
    } else if (gear_to_unequip->gear_type != TWO_HANDED) {
        inventory->equipped[target_slot] = NULL;
    } else {
        log_msg(WARNING, "Inventory", "In `unequip_gear_slot` invalid gear type %d for slot %d",
                gear_to_unequip->gear_type, target_slot);
        return -1;
    }

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

int is_gear_equipped(const Inventory* inventory, const gear_t* gear) {
    RETURN_WHEN_NULL(inventory, -1, "Inventory", "In `is_gear_equipped` inventory is NULL")
    RETURN_WHEN_NULL(gear, -1, "Inventory", "In `is_gear_equipped` given gear is NULL")

    const int gear_type = gear->gear_type;

    int is_equipped = 0;
    if (gear_type == TWO_HANDED) {
        if (inventory->equipped[OFF_HAND_SLOT] != gear &&
            inventory->equipped[MAIN_HAND_SLOT] != gear) {
            // two-handed gear is not equipped
        } else if (inventory->equipped[OFF_HAND_SLOT] == gear ||
                   inventory->equipped[MAIN_HAND_SLOT] == gear) {
            is_equipped = 1; // two-handed gear is equipped
        } else {
            log_msg(WARNING, "Inventory", "In `is_gear_equipped`"
                                          "two-handed gear is not equipped properly");
            is_equipped = -1;
        }
    } else {
        if (inventory->equipped[gear_type] == gear) {
            is_equipped = 1; // gear is equipped
        }
    }

    return is_equipped;
}
