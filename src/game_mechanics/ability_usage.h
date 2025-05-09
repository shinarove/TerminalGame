#ifndef ABILITY_USAGE_H
#define ABILITY_USAGE_H

#include "../game_data/ability/ability.h"
#include "../game_data/character/character.h"

typedef enum {
    SUCCESS,
    NOT_ENOUGH_HEALTH,
    NOT_ENOUGH_STAMINA,
    NOT_ENOUGH_MANA,
    MISSED,     // missed the target, only possible when user and target are different
    FAILED,     // failed to use ability, only possible when user and target are the same
    TARGET_DIED,// special success case for when the target died
    UNEXPECTED_ERROR
} usage_result_t;

/**
 * Executes an ability used by a character on a target character. The success
 * of the ability depends on various internal factors such as resource availability,
 * accuracy checks, and target conditions.
 *
 * @param user A pointer to the character using the ability. This must not be NULL.
 * @param target A pointer to the character being targeted by the ability. This must not be NULL.
 * @param ability A pointer to the ability being used. This must not be NULL.
 * @return A value of type `usage_result_t` indicating the result of the ability usage.
 *         Possible values include:
 *         - SUCCESS: The ability was successfully used as intended.
 *         - NOT_ENOUGH_HEALTH: The user lacks sufficient health to use the ability.
 *         - NOT_ENOUGH_STAMINA: The user lacks sufficient stamina to use the ability.
 *         - NOT_ENOUGH_MANA: The user lacks sufficient mana to use the ability.
 *         - MISSED: The ability failed as the user missed the target.
 *         - FAILED: The ability failed for reasons other than accuracy.
 *         - TARGET_DIED: The target died after using the ability, resulting in a special success state.
 *         - UNEXPECTED_ERROR: An unexpected error occurred during ability usage.
 */
usage_result_t use_ability(character_t* user, character_t* target, const ability_t* ability);

#endif//ABILITY_USAGE_H
