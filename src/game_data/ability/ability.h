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

/**
 * Creates an ability array with a predefined allocated space for abilities.
 * This function initializes an `ability_array_t` object and allocates memory
 * for it and its internal abilities array, as specified by the `pre_length` parameter.
 * The internal abilities array will be filled with `NULL` pointers initially.
 * If `pre_length` is 0, the internal abilities array will not be allocated.
 *
 * @param pre_length The number of abilities to reserve space for in the array.
 *                   Must be 0 or a non-negative integer. If negative, the function
 *                   will return `NULL`.
 * @return A pointer to the initialized `ability_array_t` object, or `NULL` if memory
 *         allocation fails or the input `pre_length` is negative.
 */
ability_array_t* create_ability_array(int pre_length);

/**
 * Adds an ability to the given ability array.
 * This function checks whether the ability array is initialized and has sufficient
 * allocated space. If the array is uninitialized, it allocates an underlying array
 * from the global memory pool. If the array is full, it reallocates memory to double
 * the current size. The specified ability, identified by its ID, is then added to
 * the ability array.
 *
 * @param ability_array A pointer to the `ability_array_t` where the ability will be added.
 *                      Must not be NULL and should have appropriately allocated memory.
 * @param ability_id The ID of the ability to be added. Must correspond to a valid ability in the ability table.
 * @return 0 on successful addition of the ability, or 1 if the function fails due to NULL pointers,
 *         insufficient allocated space, or memory allocation errors.
 */
int add_ability_a(ability_array_t* ability_array, ability_id_t ability_id);

ability_t* get_ability_a(const ability_array_t* ability_array, ability_id_t ability_id);
/**
 * Removes an ability from the specified ability array based on the given ability ID.
 *
 * This function searches the provided ability array for an ability with the specified ID.
 * If the ability is found, it is removed, and the array is adjusted to maintain contiguous
 * storage of abilities. The function ensures the input parameters are valid and reports
 * errors using logging mechanisms if invalid states are encountered.
 *
 * @param ability_array A pointer to the `ability_array_t` object from which the ability
 *                      should be removed. If this pointer is `NULL` or references invalid data,
 *                      the function will log an error and return a failure code.
 * @param ability_id    The unique identifier of the ability to be removed. Valid IDs must
 *                      be within the range `[0, MAX_ABILITIES - 1]`. Invalid IDs will result
 *                      in a logged error and termination of the operation.
 * @return Returns `0` if the ability was successfully removed. Returns `1` if no ability
 *         with the specified ID exists in the array, if the provided ability array is `NULL`,
 *         or if other invalid input states are detected.
 */
int remove_ability_a(ability_array_t* ability_array, ability_id_t ability_id);

/**
 * Frees the memory associated with the given ability array and its internal resources.
 * This function releases all dynamically allocated memory used by the ability array,
 * including the memory for the abilities pointer array and the ability array structure itself.
 *
 * @param ability_array A pointer to the `ability_array_t` structure to be destroyed.
 *                      If the pointer is `NULL`, the function will exit without any action.
 */
void destroy_ability_array(ability_array_t* ability_array);

#endif//ABILITY_H
