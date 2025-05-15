#include "inventory.h"

#include "../../logger/logger.h"

#include <stdlib.h>

inventory_t* create_empty_inventory(const memory_pool_t* pool, int pre_length) {
    RETURN_WHEN_NULL(pool, NULL, "Inventory", "In `create_empty_inventory` given memory pool is NULL")
    RETURN_WHEN_TRUE(pre_length < 0, NULL, "Inventory", "In `create_empty_inventory` allocated_space is negative")
    inventory_t* inventory = (inventory_t*) memory_pool_alloc(pool, sizeof(inventory_t));
    RETURN_WHEN_NULL(inventory, NULL, "Inventory", "In `create_empty_inventory` failed to allocate memory for inventory")

    if (pre_length == 0) {
        inventory->gears = NULL;
    } else {
        inventory->gears = (gear_t**) memory_pool_alloc(pool, sizeof(gear_t*) * pre_length);
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
    return inventory;
}
