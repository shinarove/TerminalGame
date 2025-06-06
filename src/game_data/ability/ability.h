#ifndef ABILITY_H
#define ABILITY_H

#include "../../memory/mem_mgmt.h"

typedef enum {
    //basic abilities
    BITE,
    CLAWS,
    PUNCH,
    //sword abilities
    SWORD_SLASH,
    POMMEL_STRIKE,
    MURDER_STRIKE,
    // shield abilities
    SHIELD_BASH,
    // amulette abilities
    HEAL_HEALTH,
    HEAL_STAMINA,
    HEAL_MANA,
    //...
    MAX_ABILITIES
} ability_id_t;

#define SELF_CHAR 's'
#define ENEMY_CHAR 'e'
#define HEALTH_CHAR 'h'
#define STAMINA_CHAR 's'
#define MANA_CHAR 'm'
#define DAMAGE_CHAR 'd'
#define HEAL 'h'
#define STRENGTH_CHAR 's'
#define INTELLIGENCE_CHAR 'i'
#define AGILITY_CHAR 'a'
#define CONSTITUTION_CHAR 'c'
#define LUCK_CHAR 'l'

typedef struct ability {
    int id;
    char* key_name;
    char* local_name;
    char* ability_str;
    char c_target;       // 's' for self, 'e' for enemy
    char r_target;       // 'h' for health, 's' for stamina, 'm' for mana
    char effect_type;    // 'd' for damage, 'h' for heal
    char effect_scaler;  // 's' for strength, 'i' for intelligence, 'a' for agility, 'c' for constitution, 'l' for luck
    char accuracy_scaler;// 's' for strength, 'i' for intelligence, 'a' for agility, 'c' for constitution, 'l' for luck
    char r_cost;         // 'h' for health, 's' for stamina, 'm' for mana

    int effect_dice;   // dice to calculate the base effect
    int accuracy_dice; // the used dice to determine if the user hits the target
    int effect_rolls;  // the number of rolls for the effect dice
    int accuracy_rolls;// the number of rolls for the accuracy dice

    float effect_scale_value;  // this value will be multiplied with the effect_scaler_attr and added to the effect_base_value
    float accuracy_scale_value;// this value will be multiplied with the accuracy_scaler_attr and added to the rolled accuracy value

    int v_cost;// the cost of the ability in the used resource (costs_resources)
} ability_t;

typedef struct ability_array {
    int ability_count;  // the current number of abilities in the array
    int allocated_space;// the size of the allocated space for the abilities

    ability_t** abilities;// the array of ability pointers
} ability_array_t;

typedef struct {
    ability_t abilities[MAX_ABILITIES];
    int count;
} ability_table_t;

/**
 * Initializes the ability table singleton object using the given memory pool.
 * This function creates a singleton instance of the `ability_table_t` if it does not already exist.
 * It allocates memory for the ability table from the provided memory pool and populates it
 * with ability data from a predefined file. The function will return the singleton instance
 * of the ability table.
 *
 * @param pool A pointer to the memory pool from which the ability table will be allocated.
 *             The memory pool must be properly initialized and must have sufficient space
 *             to allocate the required memory.
 * @return A pointer to the initialized `ability_table_t` object, or `NULL` if the provided memory pool is `NULL`.
 */
ability_table_t* init_ability_table(const memory_pool_t* pool);

/**
 * Retrieves the singleton instance of the ability table.
 * This function returns the pointer to the global `ability_table_t` object.
 * If the ability table has not been initialized, this will return `NULL`.
 *
 * @return A pointer to the `ability_table_t` instance, or `NULL` if it has not been initialized.
 */
ability_table_t* get_ability_table(void);

/**
 * Destroys the ability table singleton object and frees all associated memory.
 * This function cleans up the memory allocated for the ability table, including
 * the ability data stored within, and releases it back to the specified memory pool.
 * After cleanup, the ability table singleton is set to `NULL`.
 *
 * @param pool A pointer to the memory pool from which the ability table was allocated.
 *             The memory pool must be properly initialized. If the provided memory pool
 *             is `NULL`, the function logs an error and returns without doing anything.
 */
void destroy_ability_table(const memory_pool_t* pool);

#endif//ABILITY_H
