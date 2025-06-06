#ifndef ENEMY_GENERATOR_H
#define ENEMY_GENERATOR_H

#include "character.h"

/**
 * Generates a goblin character with the specified level.
 *
 * This function creates a goblin character using a base goblin template, assigns an appropriate name based on the level,
 * and upgrades the goblin's attributes and abilities according to its level. The level is capped between 1 and 20.
 * An ability "CLAWS" is added to the goblin by default. No random items are currently assigned (TODO).
 *
 * @param level The desired level of the goblin character. Values less than 1 are clamped to 1, and values greater than 20 are clamped to 20.
 * @return A pointer to the created goblin character, or NULL if the creation fails due to memory or other errors.
 */
Character* generate_goblin(int level);

#endif//ENEMY_GENERATOR_H
