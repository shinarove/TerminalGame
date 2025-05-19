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

void apply_bonus_stats_c(character_t* character, const resources_t* bonus_res, const attributes_t* bonus_att);

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

    reset_update_flags_c(character);
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

    reset_update_flags_c(character);
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

    // update the ressource flag
    if (health != 0 || stamina != 0 || mana != 0) {
        character->u_flag_resources = 1;
    }
}

void reset_resources_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "In `reset_resources_c` given character is NULL")
    character->current_resources = character->max_resources;

    // update the ressource flags
    character->u_flag_resources = 1;
}

void reset_health_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "In `reset_health_c` given character is NULL")
    character->current_resources.health = character->max_resources.health;

    // update the ressource flags
    character->u_flag_resources = 1;
}

void reset_stamina_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "In `reset_stamina_c` given character is NULL")
    character->current_resources.stamina = character->max_resources.stamina;

    // update the ressource flags
    character->u_flag_resources = 1;
}

void reset_mana_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "In `reset_mana_c` given character is NULL")
    character->current_resources.mana = character->max_resources.mana;

    // update the ressource flags
    character->u_flag_resources = 1;
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
    character->base_resources.health += character->base_attributes.constitution;
    character->base_resources.stamina += (int) ((float) character->base_attributes.agility * 0.5);
    character->base_resources.stamina += character->base_attributes.constitution;
    character->base_resources.mana += (int) ((float) character->base_attributes.intelligence * 1.5);

    character->max_resources = character->base_resources;
    character->current_resources = character->base_resources;
    // add equipment bonuses
    if (character->inventory != NULL) {
        apply_bonus_stats_c(character, &character->inventory->total_resource_bonus,
                            &character->inventory->total_attribute_bonus);
    }

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
        case CONSTITUTION:
            character->base_attributes.constitution++;
            character->max_attributes.constitution++;
            character->current_attributes.constitution++;
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

    // update the ressource and attribute flags
    character->u_flag_resources = 1;
    character->u_flag_attributes = 1;
}

int add_gear_c(character_t* character, const gear_id_t gear_id) {
    RETURN_WHEN_NULL(character, 1, "Character", "In `add_gear_c` given character is NULL")

    if (character->inventory == NULL) {
        character->inventory = create_inventory(5);
    } else if (character->max_carry_weight == character->inventory->gear_count) {
        return 1;// inventory is full
    }

    // TODO: maybe this should be allowed?
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

int equip_gear_c(character_t* character, const gear_id_t gear_id, const gear_slot_t target_slot) {
    RETURN_WHEN_NULL(character, 1, "Character", "In `equip_gear_c` given character is NULL")
    RETURN_WHEN_TRUE(target_slot < 0 || target_slot >= MAX_GEAR_SLOTS, 1,
                     "Character", "In `equip_gear_c` given target slot is invalid: %d", target_slot)

    // unequip potential gear
    if (target_slot == MAIN_HAND_SLOT || target_slot == OFF_HAND_SLOT) {
        if (character->inventory->equipped[BOTH_HAND_SLOT] != NULL) {
            unequip_gear_c(character, BOTH_HAND_SLOT);
        }
    } else if (target_slot == BOTH_HAND_SLOT) {
        if (character->inventory->equipped[MAIN_HAND_SLOT] != NULL) {
            unequip_gear_c(character, MAIN_HAND_SLOT);
        }
        if (character->inventory->equipped[OFF_HAND_SLOT] != NULL) {
            unequip_gear_c(character, OFF_HAND_SLOT);
        }
    }
    unequip_gear_c(character, target_slot);

    const int equip_success = equip_gear_i(character->inventory, gear_id, target_slot);

    if (equip_success == 0) {
        apply_bonus_stats_c(character, &character->inventory->total_resource_bonus,
                            &character->inventory->total_attribute_bonus);

        // add abilities connected to the gear
        const int ability_count = character->inventory->equipped[target_slot]->ability_count;
        for (int i = 0; i < ability_count; i++) {
            add_ability_c(character, character->inventory->equipped[target_slot]->ability_ids[i]);
        }
    }

    return equip_success;
}

int unequip_gear_c(character_t* character, const gear_slot_t target_slot) {
    RETURN_WHEN_NULL(character, 1, "Character", "In `unequip_gear_c` given character is NULL")
    RETURN_WHEN_TRUE(target_slot < 0 || target_slot >= MAX_GEAR_SLOTS, 1,
                     "Character", "In `unequip_gear_c` given target slot is invalid: %d", target_slot)

    // check if a gear is equipped in the target slot
    if (character->inventory->equipped[target_slot] == NULL) {
        return 1;
    }

    // remove abilities connected to the gear
    const int ability_count = character->inventory->equipped[target_slot]->ability_count;
    for (int i = 0; i < ability_count; i++) {
        remove_ability_c(character, character->inventory->equipped[target_slot]->ability_ids[i]);
    }

    const int unequip_success = unequip_gear_i(character->inventory, target_slot);
    apply_bonus_stats_c(character, &character->inventory->total_resource_bonus,
                        &character->inventory->total_attribute_bonus);

    // add base ability if no weapons are equipped (off hand is here excluded)
    if (character->inventory->equipped[MAIN_HAND_SLOT] == NULL &&
        character->inventory->equipped[BOTH_HAND_SLOT] == NULL) {
        add_ability_c(character, character_base_ability[character->id].basic_ability_id);
    }

    return unequip_success;
}

gear_t* get_gear_by_id_c(const character_t* character, gear_id_t gear_id) {
    RETURN_WHEN_NULL(character, NULL, "Character", "In `get_gear_by_id_c` given character is NULL")
    RETURN_WHEN_NULL(character->inventory, NULL, "Character",
                     "In `get_gear_by_id_c` given character's inventory is NULL")
    RETURN_WHEN_NULL(character->inventory->gears, NULL, "Character",
                     "In `get_gear_by_id_c` given character's inventory is not initialized")

    gear_t* gear = NULL;
    for (int i = 0; i < character->inventory->gear_count; i++) {
        if (character->inventory->gears[i]->id == (int) gear_id) {
            gear = character->inventory->gears[i];
            break;
        }
    }
    return gear;
}

int add_ability_c(character_t* character, const ability_id_t ability_id) {
    RETURN_WHEN_NULL(character, 1, "Character", "In `add_ability_c` given character is NULL")

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
        if (character->abilities->abilities[i]->id == (int) ability_id) {
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

void apply_bonus_stats_c(character_t* character, const resources_t* bonus_res, const attributes_t* bonus_att) {
    if (bonus_res == NULL || bonus_att == NULL) return;
    const resources_t prev_max_res = character->max_resources;
    const attributes_t prev_max_att = character->max_attributes;

    // reapply bonus on max stats
    character->max_resources.health = character->base_resources.health + bonus_res->health;
    character->max_resources.stamina = character->base_resources.stamina + bonus_res->stamina;
    character->max_resources.mana = character->base_resources.mana + bonus_res->mana;
    character->max_attributes.strength = character->base_attributes.strength + bonus_att->strength;
    character->max_attributes.intelligence = character->base_attributes.intelligence + bonus_att->intelligence;
    character->max_attributes.agility = character->base_attributes.agility + bonus_att->agility;
    character->max_attributes.constitution = character->base_attributes.constitution + bonus_att->constitution;
    character->max_attributes.luck = character->base_attributes.luck + bonus_att->luck;

    // add difference prev max / new max to current stats
    character->current_resources.health += character->max_resources.health - prev_max_res.health;
    character->current_resources.stamina += character->max_resources.stamina - prev_max_res.stamina;
    character->current_resources.mana += character->max_resources.mana - prev_max_res.mana;
    character->current_attributes.strength += character->max_attributes.strength - prev_max_att.strength;
    character->current_attributes.intelligence += character->max_attributes.intelligence - prev_max_att.intelligence;
    character->current_attributes.agility += character->max_attributes.agility - prev_max_att.agility;
    character->current_attributes.constitution += character->max_attributes.constitution - prev_max_att.constitution;
    character->current_attributes.luck += character->max_attributes.luck - prev_max_att.luck;

    // update the update flags
    character->u_flag_resources = 1;
    character->u_flag_attributes = 1;
}

void reset_update_flags_c(character_t* character) {
    RETURN_WHEN_NULL(character, , "Character", "In `reset_update_flags_c` given character is NULL")

    character->u_flag_resources = 0;
    character->u_flag_attributes = 0;
    character->u_flag_abilities = 0;
    character->u_flag_inventory = 0;
}
