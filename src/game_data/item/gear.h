#ifndef ITEM_H
#define ITEM_H

#include "../../memory/mem_mgmt.h"
#include "../ability/ability.h"
#include "../character/stats.h"

typedef enum {
    // main-hand
    IRON_SWORD,
    STEEL_SWORD,
    // off-hand
    IRON_SHIELD,
    STEEL_SHIELD,
    // head
    IRON_HELMET,
    STEEL_HELMET,
    // body
    IRON_ARMOR,
    STEEL_ARMOR,
    // legs
    IRON_LEGGINGS,
    STEEL_LEGGINGS,
    // hands
    IRON_GLOVES,
    STEEL_GLOVES,
    // rings
    RING_OF_STRENGTH,
    RING_OF_INTELLIGENCE,
    RING_OF_AGILITY,
    RING_OF_ENDURANCE,
    RING_OF_LUCK,
    // amulets
    AMULET_OF_HEALING_HEALTH,
    AMULET_OF_HEALING_STAMINA,
    AMULET_OF_HEALING_MANA,
    //...
    MAX_GEARS
} gear_id_t;

typedef enum {
    HEAD_ARMOR,
    BODY_ARMOR,
    LEG_ARMOR,
    HAND_ARMOR,
    RING,
    AMULET,
    MAIN_HAND,
    OFF_HAND,
    TWO_HANDED,
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
    int* ability_ids;// an array of ability IDs
} gear_t;

typedef struct gear_table {
    gear_t gears[MAX_GEARS];
    int count;
} gear_table_t;

gear_table_t* init_gear_table(const memory_pool_t* pool);

gear_table_t* get_gear_table(void);

void destroy_gear_table(const memory_pool_t* pool);

#endif//ITEM_H
