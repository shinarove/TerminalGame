#ifndef ITEM_H
#define ITEM_H

#include "../../memory/mem_mgmt.h"
#include "../character/stats.h"
#include "../ability/ability.h"

typedef enum {
    IRON_SWORD,

    MAX_GEARS
} gear_id_t;

typedef enum {
    HEAD_ARMOR,
    BODY_ARMOR,
    LEG_ARMOR,
    HAND_ARMOR,
    RING,
    AMULET,
    TOOL_ONE_HANDED,
    TOOL_TWO_HANDED,
    MAX_GEAR_TYPES
} gear_type_t;

typedef struct gear {
    int id;
    gear_type_t gear_type;
    char* key_name;
    char* local_name;

    resources_t resource_bonus;
    attributes_t attribute_bonus;

    // connected abilities
    int ability_count;
    ability_id_t* ability_ids; // an array of ability IDs
} gear_t;

typedef struct gear_table {
    gear_t* gears[MAX_GEARS];
    int num_gears;
} gear_table_t;

gear_table_t* init_gear_table(const memory_pool_t* pool);

#endif//ITEM_H
