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
    MAIN_HAND_SLOT,// the main hand slot
    OFF_HAND_SLOT, // the off hand slot
    BOTH_HAND_SLOT,// the two handed slot
    MAX_GEAR_SLOTS
} gear_slot_t;

static const struct {
    gear_slot_t slot;
    gear_type_t type;
} gear_slot_type_mapping[MAX_GEAR_SLOTS] = {
        {HEAD_SLOT, HEAD_ARMOR},
        {BODY_SLOT, BODY_ARMOR},
        {LEG_SLOT, LEG_ARMOR},
        {HAND_SLOT, HAND_ARMOR},
        {RING_LEFT_SLOT, RING},
        {RING_RIGHT_SLOT, RING},
        {AMULET_SLOT, AMULET},
        {MAIN_HAND_SLOT, MAIN_HAND},
        {OFF_HAND_SLOT, OFF_HAND},
        {BOTH_HAND_SLOT, BOTH_HAND}};

typedef struct inventory {
    int gear_count;     // the number of gears in the inventory (with the equipped gears)
    int allocated_space;// the size of the allocated space for the gears

    resources_t total_resource_bonus;  // the total resource bonus from all equipped gears
    attributes_t total_attribute_bonus;// the total attribute bonus from all equipped gears

    gear_t* equipped[MAX_GEAR_SLOTS];// the equipped gears
    gear_t** gears;                  // the array of gear pointers
} inventory_t;

/**
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
 *       and all pointers within the array are initialized to NULL.
 */
inventory_t* create_inventory(int pre_length);

/**
 * This function adds a gear item, identified by its gear ID, to the given inventory.
 * If the inventory does not have allocated memory or is full, the memory allocation
 * is either initialized with a default size or expanded as necessary to accommodate
 * the new gear. The gear count is updated accordingly.
 *
 * @param inventory A pointer to the inventory_t structure where the gear will be added.
 *                  The inventory must be valid and properly initialized.
 * @param gear_id The unique identifier for the gear to be added. It must be within a valid range
 *                (non-negative and less than MAX_GEARS).
 *
 * @return 0 if the gear was successfully added to the inventory,
 *         or 1 if an error occurs (e.g., invalid parameters, allocation issues, or state inconsistencies).
 */
int add_gear_i(inventory_t* inventory, gear_id_t gear_id);

/**
 * Removes a gear item with the specified gear ID from the inventory.
 * If the gear is found, it is removed, and the inventory is updated to
 * ensure contiguous storage of remaining gear items. If the gear is not
 * found, no changes are made to the inventory.
 *
 * @param inventory A pointer to the inventory structure from which the gear
 *                  should be removed. If this parameter is NULL, the function
 *                  will return an error code.
 * @param gear_id   The ID of the gear to be removed. If the gear ID is invalid
 *                  (e.g., does not fall within the valid range or does not
 *                  exist in the inventory), the function will return an error code.
 *
 * @return An integer indicating the success or failure of the operation:
 *         0 - The gear was successfully removed from the inventory.
 *         1 - Failure due to a NULL inventory pointer, invalid gear ID, or
 *             the gear not being found in the inventory.
 */
int remove_gear_i(inventory_t* inventory, gear_id_t gear_id);

int equip_gear_i(inventory_t* inventory, gear_id_t gear_id, gear_slot_t target_slot);

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
