#ifndef INVENTORY_H
#define INVENTORY_H

#include "../item/gear.h"

typedef enum {
    HEAD_SLOT,
    BODY_SLOT,
    LEG_SLOT,
    HAND_SLOT,
    RING_LEFT_SLOT,
    RING_RIGHT_SLOT,
    AMULET_SLOT,
    TOOL_LEFT_SLOT, // the slot for tools held in the left hand
    TOOL_RIGHT_SLOT, // the slot for tools held in the right hand
    TOOL_BOTH_SLOT, // the slot for tools held in both hands
    MAX_GEAR_SLOTS
} gear_slot_t;

typedef struct inventory {
    int gear_count; // the number of gears in the inventory (with the equipped gears)
    int allocated_space; // the size of the allocated space for the gears

    resources_t total_resource_bonus; // the total resource bonus from all equipped gears
    attributes_t total_attribute_bonus; // the total attribute bonus from all equipped gears

    gear_t* equipped[MAX_GEAR_SLOTS]; // the equipped gears
    gear_t** gears; // the array of gear pointers
} inventory_t;

/**
 * @brief Creates and initializes a new inventory structure.
 *
 * This function allocates memory for an inventory instance and optionally
 * pre-allocates an array to store gear items. It initializes the gear slots,
 * equipped gear, and sets the default values for gear count and allocated space.
 *
 * @param pre_length An integer specifying the initial allocated space for gears.
 *                   If the value is zero, no array allocation for gears is performed.
 *                   If negative, the function will return NULL.
 *
 * @return A pointer to the newly created inventory_t structure if successful,
 *         or NULL if memory allocation fails or if pre_length is negative.
 *
 * @note If `pre_length` is greater than zero, memory is allocated for `gears`
 *       and all pointers within the array are initialized to NULL. The same
 *       initialization is applied to the `equipped` slots. If `pre_length` is zero,
 *       `gears` is also set to NULL.
 */
inventory_t* create_inventory(int pre_length);

/**
 * This function releases all memory associated with the given inventory,
 * including the array of gears (if allocated). It also ensures that
 * any allocated memory in the global memory pool is properly freed.
 *
 * @param inventory A pointer to the inventory_t structure to be destroyed.
 *                  If NULL, the function does nothing.
 */
void destroy_inventory(inventory_t* inventory);


#endif//INVENTORY_H
