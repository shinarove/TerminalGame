#include "character.h"

#include "../../logger/logger.h"

#include <stdio.h>
#include <string.h>

character_t* create_base_character(const memory_pool_t* pool, const unsigned int id, const char* name) {
    RETURN_WHEN_NULL(pool, NULL, "Character", "Pool is NULL")
    RETURN_WHEN_NULL(name, NULL, "Character", "Name is NULL")

    character_t* character = memory_pool_alloc(pool, sizeof(character_t));
    RETURN_WHEN_NULL(character, NULL, "Character", "Failed to allocate memory for character")

    character->id = id;
    character->name = strdup(name);
    character->level = 1;

    const resources_t char_res = {5, 5, 5};
    character->base_resources = char_res;
    character->max_resources = char_res;
    character->current_resources = char_res;

    const attributes_t char_attr = {1, 1, 1, 1, 1};
    character->base_attributes = char_attr;
    character->max_attributes = char_attr;
    character->current_attributes = char_attr;

    return character;
}

character_t* create_character(const memory_pool_t* pool, const unsigned int id, const char* name, const int level, resources_t base_res, attributes_t base_attr) {
    RETURN_WHEN_NULL(pool, NULL, "Character", "Pool is NULL")
    RETURN_WHEN_NULL(name, NULL, "Character", "Name is NULL")

    character_t* character = memory_pool_alloc(pool, sizeof(character_t));

    character->id = id;
    character->name = strdup(name);
    character->level = (level < 1 ? 1 : level) > 20 ? 20 : level;// level can only be between 1 and 20

    //assign base resources and attributes
    character->base_resources.health = base_res.health < 1 ? 1 : base_res.health;
    character->base_resources.stamina = base_res.stamina < 1 ? 1 : base_res.stamina;
    character->base_resources.mana = base_res.mana < 1 ? 1 : base_res.mana;
    character->base_attributes.strength = base_attr.strength < 1 ? 1 : base_attr.strength;
    character->base_attributes.intelligence = base_attr.intelligence < 1 ? 1 : base_attr.intelligence;
    character->base_attributes.agility = base_attr.agility < 1 ? 1 : base_attr.agility;
    character->base_attributes.endurance = base_attr.endurance < 1 ? 1 : base_attr.endurance;
    character->base_attributes.luck = base_attr.luck < 1 ? 1 : base_attr.luck;

    //assign max & current resources and attributes
    character->max_resources = character->base_resources;
    character->current_resources = character->base_resources;

    character->max_attributes = character->base_attributes;
    character->current_attributes = character->base_attributes;

    return character;
}

void destroy_character(const memory_pool_t* pool, character_t* character) {
    free(character->name);
    memory_pool_free(pool, character);
}

void add_resources_c(character_t* character, unsigned int health, unsigned int stamina, unsigned int mana) {
    RETURN_WHEN_NULL(character, , "Character", "Character is NULL")

    const resources_t cur_res = character->current_resources;
    const resources_t max_res = character->max_resources;

    character->current_resources.health = cur_res.health + health > max_res.health ? max_res.health : cur_res.health + health;
    character->current_resources.stamina = cur_res.stamina + stamina > max_res.stamina ? max_res.stamina : cur_res.stamina + stamina;
    character->current_resources.mana = cur_res.mana + mana > max_res.mana ? max_res.mana : cur_res.mana + mana;
}

void reset_resources_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "Character is NULL")
    character->current_resources = character->max_resources;
}

void reset_health_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "Character is NULL")
    character->current_resources.health = character->max_resources.health;
}

void reset_stamina_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "Character is NULL")
    character->current_resources.stamina = character->max_resources.stamina;
}

void reset_mana_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "Character is NULL")
    character->current_resources.mana = character->max_resources.mana;
}

void level_up_c(character_t* character, attr_identifier_t attr_to_increase) {
    RETURN_WHEN_NULL(character, , "Character", "Character is NULL")
    if (character->level >= 20) {
        log_msg(WARNING, "Character", "Character is already at max level");
        return;
    }

    character->base_resources.health += (unsigned short) ((double) character->base_attributes.strength * 0.5);
    character->base_resources.health += character->base_attributes.endurance;
    character->base_resources.stamina += (unsigned short) ((double) character->base_attributes.agility * 0.5);
    character->base_resources.stamina += character->base_attributes.endurance;
    character->base_resources.mana += (unsigned short) ((double) character->base_attributes.intelligence * 1.5);

    //TODO: When inventory is implemented, add buffs from gear
    character->max_resources = character->base_resources;
    character->current_resources = character->base_resources;

    character->level++;
    switch (attr_to_increase) {
        case STRENGTH:
            character->base_attributes.strength++;
            character->max_attributes.strength++;
            character->current_attributes.strength++;
            break;
        case INTELLIGENCE:
            character->base_attributes.intelligence++;
            character->max_attributes.intelligence++;
            character->current_attributes.intelligence++;
            break;
        case AGILITY:
            character->base_attributes.agility++;
            character->max_attributes.agility++;
            character->current_attributes.agility++;
            break;
        case ENDURANCE:
            character->base_attributes.endurance++;
            character->max_attributes.endurance++;
            character->current_attributes.endurance++;
            break;
        case LUCK:
            character->base_attributes.luck++;
            character->max_attributes.luck++;
            character->current_attributes.luck++;
            break;
        default:
            log_msg(ERROR, "Character", "Invalid attribute identifier");
            break;
    }
}
