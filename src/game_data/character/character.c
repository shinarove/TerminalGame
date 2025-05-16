#include "character.h"

#include "../../game.h"
#include "../../logger/logger.h"

#include <string.h>

#define DEFAULT_LVL 1
#define DEFAULT_CURRENT_EXP 0
#define DEFAULT_UNSPENT_ATTR_P 0
#define DEFAULT_UNSPENT_RES_P 0
#define DEFAULT_MAX_CARRY_WEIGHT 10
#define DEFAULT_HEALTH 10
#define DEFAULT_STAMINA 5
#define DEFAULT_MANA 5
#define DEFAULT_STRENGTH 1
#define DEFAULT_INTELLIGENCE 1
#define DEFAULT_AGILITY 1
#define DEFAULT_ENDURANCE 1
#define DEFAULT_LUCK 1

character_t* create_empty_character() {
    character_t* character = memory_pool_alloc(global_memory_pool, sizeof(character_t));
    RETURN_WHEN_NULL(character, NULL, "Character", "Failed to allocate memory for character")

    character->id = 0;
    character->current_exp = DEFAULT_CURRENT_EXP;
    character->needed_exp = needed_exp_table[DEFAULT_LVL];
    character->level = DEFAULT_LVL;
    character->name = NULL;
    character->has_map_key = 0;
    // TODO: make this relative to the character's strength
    character->max_carry_weight = DEFAULT_MAX_CARRY_WEIGHT;

    character->unspent_attr_p = DEFAULT_UNSPENT_ATTR_P;
    character->unspent_res_p = DEFAULT_UNSPENT_RES_P;

    const resources_t char_res = {DEFAULT_HEALTH, DEFAULT_STAMINA, DEFAULT_MANA};
    character->base_resources = char_res;
    character->max_resources = char_res;
    character->current_resources = char_res;

    const attributes_t char_attr = {
            DEFAULT_STRENGTH, DEFAULT_INTELLIGENCE, DEFAULT_AGILITY,
            DEFAULT_ENDURANCE, DEFAULT_LUCK};
    character->base_attributes = char_attr;
    character->max_attributes = char_attr;
    character->current_attributes = char_attr;

    character->abilities = NULL;
    character->inventory = NULL;

    return character;
}

character_t* create_base_character(const int id, const char* name) {
    RETURN_WHEN_NULL(name, NULL, "Character", "In `create_base_character` given name is NULL")

    character_t* character = memory_pool_alloc(global_memory_pool, sizeof(character_t));
    RETURN_WHEN_NULL(character, NULL, "Character", "Failed to allocate memory for character")

    character->id = id;
    character->current_exp = DEFAULT_CURRENT_EXP;
    character->needed_exp = needed_exp_table[DEFAULT_LVL];
    character->level = DEFAULT_LVL;
    character->name = strdup(name);
    character->has_map_key = 0;
    character->max_carry_weight = DEFAULT_MAX_CARRY_WEIGHT;

    character->unspent_attr_p = DEFAULT_UNSPENT_ATTR_P;
    character->unspent_res_p = DEFAULT_UNSPENT_RES_P;

    const resources_t char_res = {DEFAULT_HEALTH, DEFAULT_STAMINA, DEFAULT_MANA};
    character->base_resources = char_res;
    character->max_resources = char_res;
    character->current_resources = char_res;

    const attributes_t char_attr = {
            DEFAULT_STRENGTH, DEFAULT_INTELLIGENCE, DEFAULT_AGILITY,
            DEFAULT_ENDURANCE, DEFAULT_LUCK};
    character->base_attributes = char_attr;
    character->max_attributes = char_attr;
    character->current_attributes = char_attr;

    // create the ability array and inventory with default sizes of 5
    character->abilities = create_ability_array(5);
    character->inventory = create_inventory(5);

    return character;
}

void destroy_character(character_t* character) {
    if (character == NULL) {
        log_msg(WARNING, "Character", "In `destroy_character` given character is NULL");
        return;
    }

    if (character->name != NULL) free(character->name);

    destroy_ability_array(character->abilities);
    destroy_inventory(character->inventory);
    memory_pool_free(global_memory_pool, character);
}

void add_resources_c(character_t* character, const int health, const int stamina, const int mana) {
    RETURN_WHEN_NULL(character, , "Character", "In `add_resources_c` given character is NULL")

    const resources_t cur_res = character->current_resources;
    const resources_t max_res = character->max_resources;

    character->current_resources.health = cur_res.health + health > max_res.health ? max_res.health : cur_res.health + health;
    character->current_resources.stamina = cur_res.stamina + stamina > max_res.stamina ? max_res.stamina : cur_res.stamina + stamina;
    character->current_resources.mana = cur_res.mana + mana > max_res.mana ? max_res.mana : cur_res.mana + mana;
}

void reset_resources_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "In `reset_resources_c` given character is NULL")
    character->current_resources = character->max_resources;
}

void reset_health_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "In `reset_health_c` given character is NULL")
    character->current_resources.health = character->max_resources.health;
}

void reset_stamina_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "In `reset_stamina_c` given character is NULL")
    character->current_resources.stamina = character->max_resources.stamina;
}

void reset_mana_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "In `reset_mana_c` given character is NULL")
    character->current_resources.mana = character->max_resources.mana;
}

int check_exp_c(const character_t* character) {
    return character->current_exp >= character->needed_exp;
}

void lvl_up_c(character_t* character, const attr_id_t attr_to_increase) {
    RETURN_WHEN_NULL(character, , "Character", "In `lvl_up_c` given character is NULL")
    RETURN_WHEN_TRUE(attr_to_increase == MAX_ATTRIBUTES, , "Character",
                     "Given attribute id in `lvl_up_c` == MAX_ATTRIBUTES. Now returning")
    if (character->level >= 20) {
        log_msg(INFO, "Character", "Character is already at max level");
        return;
    }

    character->base_resources.health += (int) ((float) character->base_attributes.strength * 0.5);
    character->base_resources.health += character->base_attributes.endurance;
    character->base_resources.stamina += (int) ((float) character->base_attributes.agility * 0.5);
    character->base_resources.stamina += character->base_attributes.endurance;
    character->base_resources.mana += (int) ((float) character->base_attributes.intelligence * 1.5);

    //TODO: When inventory is implemented, add buffs from gear
    character->max_resources = character->base_resources;
    character->current_resources = character->base_resources;

    character->level++;
    // ensure that exp over the limit is not lost
    character->current_exp = character->current_exp - character->needed_exp > 0 ? character->current_exp - character->needed_exp : 0;
    if (character->level == 20) character->current_exp = 0;// max level reached
    character->needed_exp = needed_exp_table[character->level];

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
            //general error handling, when the given attribute id is not supported
            log_msg(ERROR, "Character", "In `lvl_up_c` given attribute id %d is not supported", attr_to_increase);
            break;
    }
}

int add_gear_c(character_t* character, const gear_id_t gear_id) {
    RETURN_WHEN_NULL(character, , "Character", "In `add_gear_c` given character is NULL")

    if (character->inventory == NULL) {
        character->inventory = create_inventory(5);
    } else if (character->max_carry_weight == character->inventory->gear_count) {
        return 1;// inventory is full
    }

    // check if the gear is already in the inventory
    if (get_gear_by_id_c(character, gear_id) != NULL) {
        log_msg(INFO, "Character",
                "In `add_gear_c` gear %d is already in the character's inventory", gear_id);
        return 1;
    }

    return add_gear_i(character->inventory, gear_id);
}

int remove_gear_c(const character_t* character, const gear_id_t gear_id) {
    RETURN_WHEN_NULL(character, 1, "Character", "In `remove_gear_c` given character is NULL")

    return remove_gear_i(character->inventory, gear_id);
}

gear_t* get_gear_by_id_c(const character_t* character, gear_id_t gear_id) {
    RETURN_WHEN_NULL(character, NULL, "Character", "In `get_gear_by_id_c` given character is NULL")
    RETURN_WHEN_NULL(character->inventory, NULL, "Character",
                     "In `get_gear_by_id_c` given character's inventory is NULL")
    RETURN_WHEN_NULL(character->inventory->gears, NULL, "Character",
                     "In `get_gear_by_id_c` given character's inventory is not initialized")

    gear_t* gear = NULL;
    for (int i = 0; i < character->inventory->gear_count; i++) {
        if (character->inventory->gears[i]->id == gear_id) {
            gear = character->inventory->gears[i];
            break;
        }
    }
    return gear;
}

int add_ability_c(character_t* character, const ability_id_t ability_id) {
    RETURN_WHEN_NULL(character, , "Character", "In `add_ability_c` given character is NULL")

    if (character->abilities == NULL) {
        character->abilities = create_ability_array(5);
    }

    // check if the ability is already in the array
    if (get_ability_by_id_c(character, ability_id) != NULL) {
        log_msg(INFO, "Character",
                "In `add_ability_c` ability %d is already in the character's ability array", ability_id);
        return 1;
    }

    return add_ability_a(character->abilities, ability_id);
}

int remove_ability_c(const character_t* character, const ability_id_t ability_id) {
    RETURN_WHEN_NULL(character, 0, "Character", "In `remove_ability_c` given character is NULL")

    return remove_ability_a(character->abilities, ability_id);
}

ability_t* get_ability_by_id_c(const character_t* character, const ability_id_t ability_id) {
    RETURN_WHEN_NULL(character, NULL, "Character", "In `get_ability_by_id_c` given character is NULL")
    RETURN_WHEN_NULL(character->abilities, NULL, "Character",
                     "In `get_ability_by_index_c` given character's ability array is NULL")
    RETURN_WHEN_NULL(character->abilities->abilities, NULL, "Character",
                     "In `get_ability_by_index_c` given character's ability array is not initialized")

    ability_t* ability = NULL;
    for (int i = 0; i < character->abilities->ability_count; i++) {
        if (character->abilities->abilities[i]->id == ability_id) {
            ability = character->abilities->abilities[i];
            break;
        }
    }
    return ability;
}

ability_t* get_ability_by_index_c(const character_t* character, const int index) {
    RETURN_WHEN_NULL(character, NULL, "Character", "In `get_ability_by_index_c` given character is NULL")
    RETURN_WHEN_NULL(character->abilities, NULL, "Character",
                     "In `get_ability_by_index_c` given character's ability array is NULL")
    RETURN_WHEN_NULL(character->abilities->abilities, NULL, "Character",
                     "In `get_ability_by_index_c` given character's ability array is not initialized")
    RETURN_WHEN_TRUE(index < 0 || index >= character->abilities->ability_count, NULL,
                     "Character", "In `get_ability_by_index_c` given index is invalid: %d", index)

    return character->abilities->abilities[index];
}
