#include "character.h"

#include "../../logger/logger.h"
#include "enemy_id.h"
#include "../ability/ability.h"

#include <string.h>
#include <stdlib.h>

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

void reset_health_c(Character* self);
void reset_stamina_c(Character* self);
void reset_mana_c(Character* self);
int xp_limit_reached_c(const Character* self);
void lvl_up_c(Character* self, attr_id_t attr_to_increase);
int pick_up_gear_c(Character* self, const gear_t* gear);
int drop_gear_c(Character* self, const gear_t* gear);
int equip_gear_c(Character* self, const gear_t* gear);
int unequip_gear_c(Character* self, gear_slot_t target_slot);
gear_t* get_gear_at_c(const Character* self, int index);
int is_gear_equipped_c(const Character* self, const gear_t* gear);
int add_ability_c(const Character* self, const ability_t* ability);
int remove_ability_c(const Character* self, const ability_t* ability);
ability_t* get_ability_at_c(const Character* self, int index);

void apply_bonus_stats(Character* character, const resources_t* bonus_res, const attributes_t* bonus_att);

static const struct {
    int id;
    ability_id_t basic_ability_id;// when no weapons are equipped, use these abilities
} character_base_ability[] = {
        {0, PUNCH},
        {GOBLIN, CLAWS}};

static const Character_VTable character_vtable = {
        .reset_health = reset_health_c,
        .reset_stamina = reset_stamina_c,
        .reset_mana = reset_mana_c,
        .xp_limit_reached = xp_limit_reached_c,
        .lvl_up = lvl_up_c,
        .pick_up_gear = pick_up_gear_c,
        .drop_gear = drop_gear_c,
        .equip_gear = equip_gear_c,
        .unequip_gear = unequip_gear_c,
        .get_gear_at = get_gear_at_c,
        .is_gear_equipped = is_gear_equipped_c,
        .add_ability = add_ability_c,
        .remove_ability = remove_ability_c,
        .get_ability_at = get_ability_at_c};

Character* create_empty_character(const int id) {
    Character* character = malloc(sizeof(Character));
    RETURN_WHEN_NULL(character, NULL, "Character", "Failed to allocate memory for character")

    // pre-initialize all pointers to ability list and inventory to NULL
    character->ability_list = NULL;
    character->inventory = NULL;

    character->id = id;
    character->current_exp = DEFAULT_CURRENT_EXP;
    character->needed_exp = needed_exp_table[DEFAULT_LVL];
    character->level = DEFAULT_LVL;
    character->name = NULL;
    character->has_map_key = 0;
    character->current_carry_weight = 0;
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

    character->ability_list = create_array_list(sizeof(ability_t*), 0);
    RETURN_WHEN_NULL_CLEAN(character->ability_list, NULL, destroy_character(character),
                           "Character", "Failed to allocate memory for character array list")
    character->inventory = create_inventory(0);
    RETURN_WHEN_NULL_CLEAN(character->inventory, NULL, destroy_character(character),
                           "Character", "Failed to create inventory for character inventory")

    character->vtable = &character_vtable;

    // add the basic abilities to the character
    const ability_t* base_ability = &get_ability_table()->abilities[character_base_ability[id].basic_ability_id];
    const int add_success = character->vtable->add_ability(character, base_ability);
    RETURN_WHEN_TRUE_CLEAN(add_success == -1, NULL, destroy_character(character),
                           "Character", "Failed to add base ability to character")

    return character;
}

Character* create_base_character(const int id, const char* name) {
    RETURN_WHEN_NULL(name, NULL, "Character", "In `create_base_character` given name is NULL")

    Character* character = create_empty_character(id);
    if (character == NULL) return NULL;

    character->name = strdup(name);

    return character;
}

void destroy_character(Character* character) {
    if (character == NULL) {
        log_msg(WARNING, "Character", "In `destroy_character` given character is NULL");
        return;
    }

    if (character->name != NULL) {
        free(character->name);
    }
    if (character->ability_list != NULL) {
        destroy_array_list(character->ability_list);
    }
    if (character->inventory != NULL) {
        destroy_inventory(character->inventory);
    }
    free(character);
}

void reset_health_c(Character* self) {
    RETURN_WHEN_NULL(self, , "Character", "In `reset_health` given character is NULL")
    self->current_resources.health = self->max_resources.health;
}

void reset_stamina_c(Character* self) {
    RETURN_WHEN_NULL(self, , "Character", "In `reset_stamina` given character is NULL")
    self->current_resources.stamina = self->max_resources.stamina;
}

void reset_mana_c(Character* self) {
    RETURN_WHEN_NULL(self, , "Character", "In `reset_mana` given character is NULL")
    self->current_resources.mana = self->max_resources.mana;
}

int xp_limit_reached_c(const Character* self) {
    return self->current_exp >= self->needed_exp;
}

void lvl_up_c(Character* self, const attr_id_t attr_to_increase) {
    RETURN_WHEN_NULL(self, , "Character", "In `lvl_up` given character is NULL")
    RETURN_WHEN_TRUE(attr_to_increase >= MAX_ATTRIBUTES, , "Character",
                     "Given attribute id in `lvl_up` is >= as MAX_ATTRIBUTES. Now returning")
    if (self->level >= 20) {
        log_msg(INFO, "Character", "Character is already at max level");
        return;
    }

    self->base_resources.health += (int) ((float) self->base_attributes.strength * 0.5);
    self->base_resources.health += self->base_attributes.constitution;
    self->base_resources.stamina += (int) ((float) self->base_attributes.agility * 0.5);
    self->base_resources.stamina += self->base_attributes.constitution;
    self->base_resources.mana += (int) ((float) self->base_attributes.intelligence * 1.5);

    self->max_resources = self->base_resources;
    self->current_resources = self->base_resources;
    // add equipment bonuses
    if (self->inventory != NULL) {
        apply_bonus_stats(self, &self->inventory->total_resource_bonus,
                          &self->inventory->total_attribute_bonus);
    }

    self->level++;
    // ensure that exp over the limit is not lost
    self->current_exp = self->current_exp - self->needed_exp > 0 ? self->current_exp - self->needed_exp : 0;
    if (self->level == 20) self->current_exp = 0;// max level reached
    self->needed_exp = needed_exp_table[self->level];

    switch (attr_to_increase) {
        case STRENGTH:
            self->base_attributes.strength++;
            self->max_attributes.strength++;
            self->current_attributes.strength++;
            self->max_carry_weight++;// increase max carry weight by 1
            break;
        case INTELLIGENCE:
            self->base_attributes.intelligence++;
            self->max_attributes.intelligence++;
            self->current_attributes.intelligence++;
            break;
        case AGILITY:
            self->base_attributes.agility++;
            self->max_attributes.agility++;
            self->current_attributes.agility++;
            break;
        case CONSTITUTION:
            self->base_attributes.constitution++;
            self->max_attributes.constitution++;
            self->current_attributes.constitution++;
            break;
        case LUCK:
            self->base_attributes.luck++;
            self->max_attributes.luck++;
            self->current_attributes.luck++;
            break;
        default:;
    }
}

int pick_up_gear_c(Character* self, const gear_t* gear) {
    RETURN_WHEN_NULL(self, -1, "Character", "In `pick_up_gear` given character is NULL")
    RETURN_WHEN_NULL(gear, -1, "Character", "In `pick_up_gear` given gear is NULL")
    RETURN_WHEN_TRUE(self->current_carry_weight > self->max_carry_weight, -1, "Character",
                     "In `pick_up_gear` character's current carry weight is greater than max carry weight: "
                     "%d > %d",
                     self->current_carry_weight, self->max_carry_weight)
    if (self->current_carry_weight == self->max_carry_weight) {
        // inventory is full, cannot pick up gear
        return 1;
    }

    // try to add the gear to the inventory
    const int add_success = self->inventory->vtable->add_gear(self->inventory, gear);
    if (add_success == 0) {
        // successfully added the gear to the inventory
        self->current_carry_weight += 1;
    }
    return add_success;
}

int drop_gear_c(Character* self, const gear_t* gear) {
    RETURN_WHEN_NULL(self, -1, "Character", "In `drop_gear` given character is NULL")
    RETURN_WHEN_NULL(gear, -1, "Character", "In `drop_gear` given gear is NULL")

    // try to remove the gear from the inventory
    const int remove_success = self->inventory->vtable->remove_gear(self->inventory, gear);
    if (remove_success == 0) {
        // successfully removed the gear from the inventory
        self->current_carry_weight -= 1;
    }
    return remove_success;
}

int equip_gear_c(Character* self, const gear_t* gear) {
    RETURN_WHEN_NULL(self, -1, "Character", "In `equip_gear` given character is NULL")
    RETURN_WHEN_NULL(gear, -1, "Character", "In `equip_gear` given gear is NULL")

    // unequip any equipped gear in the target slot
    int unequip_success = 0;
    if (gear->gear_type == TWO_HANDED) {
        // if the gear is two-handed, unequip both main and off hand slots
        unequip_success = self->vtable->unequip_gear(self, MAIN_HAND_SLOT);
        unequip_success = unequip_success != -1 ? self->vtable->unequip_gear(self, OFF_HAND_SLOT) : unequip_success;
    } else {
        self->vtable->unequip_gear(self, gear->gear_type);
    }
    if (unequip_success < 0) {
        return unequip_success;
    }

    const gear_slot_t target_slot = gear->gear_type == TWO_HANDED ? MAIN_HAND_SLOT : gear->gear_type;
    const int equip_success = self->inventory->vtable->equip_gear(self->inventory, gear);
    if (equip_success == 0) {
        apply_bonus_stats(self, &self->inventory->total_resource_bonus,
                          &self->inventory->total_attribute_bonus);

        // add abilities connected to the gear
        for (int i = 0; i < gear->ability_count; i++) {
            self->vtable->add_ability(self, self->inventory->equipped[target_slot]->abilities[i]);
        }
    }
    return equip_success;
}

int unequip_gear_c(Character* self, const gear_slot_t target_slot) {
    RETURN_WHEN_NULL(self, -1, "Character", "In `unequip_gear` given character is NULL")
    RETURN_WHEN_TRUE(target_slot >= MAX_GEAR_SLOTS, -1, "Character",
                     "In `unequip_gear` given target slot is invalid: %d", target_slot)

    if (self->inventory->equipped[target_slot] == NULL) return 1;// no gear to unequip in the target slot
    const gear_t* gear_to_unequip = self->inventory->equipped[target_slot];

    const int unequip_success = self->inventory->vtable->unequip_gear(self->inventory, target_slot);
    if (unequip_success == 0) {
        apply_bonus_stats(self, &self->inventory->total_resource_bonus,
                          &self->inventory->total_attribute_bonus);

        // remove abilities connected to the gear
        for (int i = 0; i < gear_to_unequip->ability_count; i++) {
            self->vtable->remove_ability(self, gear_to_unequip->abilities[i]);
        }


        // check if there is still a main hand or off hand gear equipped
        if (target_slot == MAIN_HAND_SLOT || target_slot == OFF_HAND_SLOT) {
            if (self->inventory->equipped[MAIN_HAND_SLOT] == NULL && self->inventory->equipped[OFF_HAND_SLOT] == NULL) {
                // no weapons a equipped, so add the basic ability
                const ability_id_t basic_ability_id = character_base_ability[self->id].basic_ability_id;
                const ability_t* ability_to_add = &get_ability_table()->abilities[basic_ability_id];
                self->vtable->add_ability(self, ability_to_add);
            }
        }
    }
    return unequip_success;
}

gear_t* get_gear_at_c(const Character* self, const int index) {
    RETURN_WHEN_NULL(self, NULL, "Character", "In `get_gear_at` given character is NULL")

    return self->inventory->vtable->get_gear_at(self->inventory, index);
}

int is_gear_equipped_c(const Character* self, const gear_t* gear) {
    RETURN_WHEN_NULL(self, -1, "Character", "In `is_gear_equipped` given character is NULL")
    RETURN_WHEN_NULL(gear, -1, "Character", "In `is_gear_equipped` given gear is NULL")

    return self->inventory->vtable->is_gear_equipped(self->inventory, gear);
}

int add_ability_c(const Character* self, const ability_t* ability) {
    RETURN_WHEN_NULL(self, -1, "Character", "In `add_ability` given character is NULL")
    RETURN_WHEN_NULL(ability, -1, "Character", "In `add_ability` given ability is NULL")

    // check if the ability is already in the array
    if (self->ability_list->vtable->list->find(self, &ability) == -1) {
        // ability is already in the array list
        log_msg(INFO, "Character", "In `add_ability` ability %s is already in the character's ability list",
                ability->local_name);
        return 1;
    }

    return self->ability_list->vtable->list->add(self->ability_list, &ability);
}

int remove_ability_c(const Character* self, const ability_t* ability) {
    RETURN_WHEN_NULL(self, -1, "Character", "In `remove_ability` given character is NULL")
    RETURN_WHEN_NULL(ability, -1, "Character", "In `remove_ability` given ability is NULL")

    return self->ability_list->vtable->list->remove(self->ability_list, &ability);
}

ability_t* get_ability_at_c(const Character* self, const int index) {
    RETURN_WHEN_NULL(self, NULL, "Character", "In `get_ability_at` given character is NULL")

    const void* ptr = self->ability_list->vtable->list->get(self->ability_list, index);
    RETURN_WHEN_NULL(ptr, NULL, "Character", "In `get_ability_at` given index is out of bounds: %d", index)

    return *(ability_t**) ptr;
}

void apply_bonus_stats(Character* character, const resources_t* bonus_res, const attributes_t* bonus_att) {
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
}
