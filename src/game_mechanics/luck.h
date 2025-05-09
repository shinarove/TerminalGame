#ifndef LUCK_H
#define LUCK_H

#include "../game_data/character/character.h"

/**
 * Rolls a luck-based die for a given character and returns the result.
 *
 * The dice used is determined by the character's current luck attribute. The mapping of
 * the character's luck to the dice is as follows:
 * - Greater than 24: D20
 * - Between 21 and 24: D12
 * - Between 17 and 20: D10
 * - Between 13 and 16: D8
 * - Between 9 and 12: D6
 * - Between 5 and 8: D4
 * - Between 1 and 4: D3
 * - Less than 1: Invalid luck, returns 0.
 *
 * If the provided character pointer is null, a log entry is recorded, and the function
 * returns 0. A warning is also logged if the character's luck is invalid (less than 1).
 *
 * @param character A pointer to the character whose luck determines the dice to roll.
 * @returns The result of the dice roll minus one (allowing a result of 0), or 0 if:
 *          - The character pointer is null.
 *          - The character has an invalid luck value.
 */
int roll_luck_dice(const character_t* character);

#endif//LUCK_H
