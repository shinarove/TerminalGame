#ifndef ENEMY_GENERATOR_H
#define ENEMY_GENERATOR_H

#include "../../memory/mem_mgmt.h"
#include "character.h"

enum enemy_id {
    // player id = 0

    GOBLIN = 1,
};


/**
 * Generates a goblin character with attributes and level scaling.
 *
 * This function creates and returns a goblin character object. The level of the goblin
 * dictates its attributes' growth, which are adjusted using a predefined level-up table.
 * The level value is clamped between 1 and 20. If the memory pool is NULL or other
 * errors occur during creation, the function returns NULL.
 *
 * @param pool A pointer to the memory pool used for allocating the goblin character.
 *             The pool must be valid and initialized.
 * @param level An integer representing the desired level of the goblin. The value is
 *              clamped between 1 (minimum) and 20 (maximum).
 * @return A pointer to the created goblin character structure on success, or NULL if
 *         the operation fails (e.g., invalid memory pool or memory allocation error).
 */
character_t* generate_goblin(const memory_pool_t* pool, unsigned int level);

#endif//ENEMY_GENERATOR_H
