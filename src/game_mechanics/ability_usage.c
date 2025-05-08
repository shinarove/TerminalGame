#include "ability_usage.h"

#include "../logger/logger.h"
#include "dice/dice.h"

/**
 * Consumes the required resource from the user character based on the given ability.
 *
 * This function deducts the resource cost of the specified ability from the user's current resources
 * (e.g., health, stamina, or mana). If the user does not have sufficient resources, the operation fails
 * and an appropriate status is returned.
 *
 * @param user A pointer to the character that is using the ability. Must not be NULL.
 * @param ability A pointer to the ability being used. Must not be NULL.
 * @return A usage_result_t indicating the status of the operation. Returns SUCCESS if the resource
 *         was consumed successfully. Returns NOT_ENOUGH_HEALTH, NOT_ENOUGH_STAMINA, or NOT_ENOUGH_MANA
 *         if the user does not have sufficient health, stamina, or mana, respectively. Returns
 *         UNEXPECTED_ERROR if an invalid resource cost type is encountered.
 */
usage_result_t consume_resource(character_t* user, const ability_t* ability);

/**
 * Evaluates the accuracy of an ability when used by a character on a target.
 *
 * This function calculates whether the user successfully lands the ability on the target based on
 * the ability's accuracy properties, the characters' agility attributes, random dice rolls, and
 * the accuracy scaler. If the user and target are the same, additional conditions are used to
 * determine success or failure.
 *
 * @param user A pointer to the character using the ability. Must not be NULL.
 * @param target A pointer to the character targeted by the ability. Must not be NULL.
 * @param ability A pointer to the ability being evaluated. Must not be NULL.
 * @return A usage_result_t indicating the result of the accuracy evaluation. Returns SUCCESS if
 *         the ability successfully lands on the target. Returns MISSED if the accuracy check fails
 *         for a different target. Returns FAILED if the ability used on the user fails.
 *         Returns UNEXPECTED_ERROR in cases of invalid scaler values.
 */
usage_result_t evaluate_accuracy(const character_t* user, const character_t* target, const ability_t* ability);

/**
 * Uses the specified ability on the target character, applying its effects.
 *
 * This function executes the ability declared by the user character upon the target character.
 * It calculates the total effect of the ability based on dice rolls and scaling values,
 * then applies the effect as damage or healing depending on the ability's effect type.
 * If an invalid effect type is encountered, the operation fails with an UNEXPECTED_ERROR status.
 *
 * @param user A pointer to the character using the ability. Must not be NULL.
 * @param target A pointer to the character targeted by the ability. Must not be NULL.
 * @param ability A pointer to the ability being used. Must not be NULL.
 * @return A usage_result_t indicating the result of the ability usage. Possible values include:
 *         SUCCESS if the ability was applied successfully, TARGET_DIED if the target was defeated,
 *         FAILED if the effect failed to execute, and UNEXPECTED_ERROR for an invalid effect type.
 */
usage_result_t use_ability_on(const character_t* user, character_t* target, const ability_t* ability);

/**
 * Retrieves the scaler value from the given character based on the provided scaler character.
 *
 * This function extracts a specific attribute value from the character's current attributes
 * (e.g., strength, intelligence, agility, etc.) based on the scaler character. If the scaler
 * character does not match any valid attribute type, the function logs a warning and returns -1.
 *
 * @param user A pointer to the character from which the scaler value is to be retrieved. Must not be NULL.
 * @param scaler_char A character representing the attribute to retrieve. Expected valid values include:
 *                    - STRENGTH_CHAR for strength
 *                    - INTELLIGENCE_CHAR for intelligence
 *                    - AGILITY_CHAR for agility
 *                    - ENDURANCE_CHAR for endurance
 *                    - LUCK_CHAR for luck
 * @return The corresponding scaler value as an integer if the scaler character is valid. Returns -1 if
 *         an invalid scaler character is encountered.
 */
int get_scaler_value(const character_t* user, char scaler_char);

/**
 * Applies damage to a specified target's resource (health, stamina, or mana), reducing the resource
 * by the given damage value. If the resource is depleted to zero, an appropriate status is returned.
 *
 * @param target A pointer to the character receiving the damage. Must not be NULL.
 * @param r_target A character representing the resource to target. Must be one of the predefined
 *                 constants: HEALTH_CHAR ('h'), STAMINA_CHAR ('s'), or MANA_CHAR ('m').
 * @param damage The amount of damage to apply to the specified resource. Must be a non-negative integer.
 * @return A usage_result_t indicating the outcome of the operation. Returns:
 *         - SUCCESS if the resource was successfully reduced without being fully depleted.
 *         - TARGET_DIED if the health resource was reduced to zero.
 *         - UNEXPECTED_ERROR if an invalid resource type was specified.
 */
usage_result_t damage_target(character_t* target, char r_target, int damage);

/**
 * Heals the specified resource of the target character by the provided amount.
 *
 * This function increases the target character's specified resource (e.g., health, stamina, or mana)
 * by the given healing amount. The resulting resource value will not exceed the target's maximum
 * resource capacity. If an invalid resource target is provided, the operation fails.
 *
 * @param target A pointer to the character whose resource will be healed. Must not be NULL.
 * @param r_target The resource to heal, represented as a character. Valid values are:
 *                 - 'h' for health
 *                 - 's' for stamina
 *                 - 'm' for mana
 * @param healing The amount of healing to apply to the specified resource. Must be a non-negative integer.
 * @return A usage_result_t indicating the status of the operation. Returns SUCCESS if the resource was
 *         healed successfully. Returns UNEXPECTED_ERROR if an invalid resource target is provided.
 */
usage_result_t heal_target(character_t* target, char r_target, int healing);

usage_result_t use_ability(character_t* user, character_t* target, const ability_t* ability) {
    RETURN_WHEN_NULL(user, 1, "Ability Usage", "User is NULL")
    RETURN_WHEN_NULL(target, 1, "Ability Usage", "Target is NULL")
    RETURN_WHEN_NULL(ability, 1, "Ability Usage", "Ability is NULL")

    usage_result_t res = consume_resource(user, ability);
    if (res == SUCCESS) {
        switch (ability->c_target) {
            case SELF_CHAR:
                res = evaluate_accuracy(user, user, ability);
                if (res == SUCCESS) {
                    res = use_ability_on(user, user, ability);
                }
                break;
            case ENEMY_CHAR:
                res = evaluate_accuracy(user, target, ability);
                if (res == SUCCESS) {
                    res = use_ability_on(user, target, ability);
                }
                break;
            default:
                log_msg(WARNING, "Ability Usage",
                        "Invalid target type: %c encountered in `use_ability`", ability->c_target);
                res = UNEXPECTED_ERROR;// failed to use ability with an unexpected error
                break;
        }
    }

    return res;
}

usage_result_t consume_resource(character_t* user, const ability_t* ability) {
    usage_result_t res = SUCCESS;
    switch (ability->r_cost) {
        case HEALTH_CHAR:
            if (user->current_resources.health < ability->v_cost) {
                res = NOT_ENOUGH_HEALTH; // failed to use ability
            } else {
                user->current_resources.health -= ability->v_cost;
            }
            break;
        case STAMINA_CHAR:
            if (user->current_resources.stamina < ability->v_cost) {
                res = NOT_ENOUGH_STAMINA; // failed to use ability
            } else {
                user->current_resources.stamina -= ability->v_cost;
            }
            break;
        case MANA_CHAR:
            if (user->current_resources.mana < ability->v_cost) {
                res = NOT_ENOUGH_MANA; // failed to use ability
            } else {
                user->current_resources.mana -= ability->v_cost;
            }
            break;
        default:
            log_msg(WARNING, "Ability Usage",
                "Invalid resource cost type: %c encountered in `consume_resource`", ability->r_cost);
            res = UNEXPECTED_ERROR; // failed to use ability
            break;
    }

    return res;
}

usage_result_t evaluate_accuracy(const character_t* user, const character_t* target, const ability_t* ability) {
    // define if the user is at an advantage: 1 for yes, 0 for no
    const int user_advantage = user->current_attributes.agility >= target->current_attributes.agility ? 1 : 0;
    // target only rolls once
    const int target_roll = roll_dice(ability->accuracy_dice);

    int user_roll = 0;
    // roll the dice based on the number of accuracy rolls, then only pick the highest
    for (int i = 0; i < ability->accuracy_rolls; i++) {
        const int roll = roll_dice(ability->accuracy_dice);
        if (user_advantage) {
            if (roll > user_roll) {
                user_roll = roll;
            }
        } else {
            if (roll < user_roll) {
                user_roll = roll;
            }
        }
    }
    const int scaler = get_scaler_value(user, ability->accuracy_scaler);
    if (scaler < 1) return UNEXPECTED_ERROR;
    // for compiler the casts aren't necessary, but makes it more predictable
    user_roll += (int) ((float) scaler * ability->accuracy_scale_value);
    if (user == target) {
        // if the user and target are the same, the ability is used on the user himself
        return user_roll > target_roll ? SUCCESS : FAILED;
    }
    return user_roll > target_roll ? SUCCESS : MISSED;
}

usage_result_t use_ability_on(const character_t* user, character_t* target, const ability_t* ability) {
    int roll_total = 0;
    for (int i = 0; i < ability->effect_rolls; i++) {
        roll_total += roll_dice(ability->effect_dice);
    }

    const int scaler = get_scaler_value(user, ability->effect_scaler);
    if (scaler < 1) return UNEXPECTED_ERROR;

    usage_result_t res = SUCCESS;
    roll_total += (int) ((float) scaler * ability->effect_scale_value);
    switch (ability->effect_type) {
        case DAMAGE_CHAR:
            res = damage_target(target, ability->r_target, roll_total);
            break;
        case HEAL:
            res = heal_target(target, ability->r_target, roll_total);
            break;
        default:
            log_msg(WARNING, "Ability Usage",
                "Invalid effect type: %c encountered in `use_ability_on`", ability->effect_type);
            return UNEXPECTED_ERROR;
    }

    return res;
}

int get_scaler_value(const character_t* user, const char scaler_char) {
    switch (scaler_char) {
        case STRENGTH_CHAR:
            return user->current_attributes.strength;
        case INTELLIGENCE_CHAR:
            return user->current_attributes.intelligence;
        case AGILITY_CHAR:
            return user->current_attributes.agility;
        case ENDURANCE_CHAR:
            return user->current_attributes.endurance;
        case LUCK_CHAR:
            return user->current_attributes.luck;
        default:
            log_msg(WARNING, "Ability Usage",
                    "Invalid scaler type: %c encountered in `get_scaler_value`", scaler_char);
            return -1;
    }
}

usage_result_t damage_target(character_t* target, const char r_target, const int damage) {
    usage_result_t res = SUCCESS;
    switch (r_target) {
        case HEALTH_CHAR:
            const unsigned short curr_health = target->current_resources.health;
            target->current_resources.health = damage > curr_health ? 0 : curr_health - damage;
            if (target->current_resources.health == 0) res = TARGET_DIED;
            break;
        case STAMINA_CHAR:
            const unsigned short curr_stamina = target->current_resources.stamina;
            target->current_resources.stamina = damage > curr_stamina ? 0 : curr_stamina - damage;
            break;
        case MANA_CHAR:
            const unsigned short curr_mana = target->current_resources.mana;
            target->current_resources.mana = damage > curr_mana ? 0 : curr_mana - damage;
            break;
        default:
            log_msg(WARNING, "Ability Usage",
                "Invalid resource target: %c encountered in `use_ability_on`", r_target);
            return UNEXPECTED_ERROR;
    }
    return res;
}

usage_result_t heal_target(character_t* target, const char r_target, const int healing) {
    switch (r_target) {
        case HEALTH_CHAR:
            target->current_resources.health = target->current_resources.health + healing > target->max_resources.health ?
                target->max_resources.health : target->current_resources.health + healing;
            break;
        case STAMINA_CHAR:
            target->current_resources.stamina = target->current_resources.stamina + healing > target->max_resources.stamina ?
                target->max_resources.stamina : target->current_resources.stamina + healing;
            break;
        case MANA_CHAR:
            target->current_resources.mana = target->current_resources.mana + healing > target->max_resources.mana ?
                target->max_resources.mana : target->current_resources.mana + healing;
            break;
        default:
            log_msg(WARNING, "Ability Usage",
                "Invalid resource target: %c encountered in `use_ability_on`", r_target);
            return UNEXPECTED_ERROR;
    }
    return SUCCESS;
}
