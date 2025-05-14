#ifndef INVENTORY_H
#define INVENTORY_H

#include "../../memory/mem_mgmt.h"
#include "../item/item.h"

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
    int item_count; // the number of items in the inventory (with the equipped items)
    int max_items; // the maximum number of items in the inventory

    item_t* equipped[MAX_GEAR_SLOTS]; // the equipped items
    item_t** items; // the array of item pointers
} inventory_t;

/**
 * Creates a new, empty inventory using the provided memory pool and a specified pre-allocation length.
 *
 * @param pool The memory pool to use for allocating the inventory and its associated resources.
 *             This parameter must not be NULL.
 * @param pre_length The initial number of item slots to pre-allocate in the inventory.
 *                   Must be greater than or equal to 0.
 * @return A pointer to the newly created inventory, or NULL if memory allocation fails
 *         or if invalid parameters are provided.
 *
 * @note If pre_length is 0, no initial item slots will be allocated, and the 'items' pointer
 *       in the inventory will be set to NULL.
 * @note All equipped item slots in the inventory will be initialized to NULL.
 * @note The item count will be initialized to 0, and the maximum number of items will
 *       be set to the value of pre_length.
 */
inventory_t* create_empty_inventory(const memory_pool_t* pool, int pre_length);


#endif//INVENTORY_H
