#include "character_save_handler.h"

#include "../../logger/logger.h"
#include "../ability/ability.h"

#include <string.h>

int write_character_data(FILE* file, const character_t* character) {
    RETURN_WHEN_NULL(file, 1, "Character Save Handler", "In `write_character_data` given file is NULL")
    RETURN_WHEN_NULL(character, 1, "Character Save Handler", "In `write_character_data` given character is NULL")

    // write id, current_exp, needed_exp, level, has_map_key,
    // unspent_attr_p, unspent_res_p, max_carry_weight
    fwrite(&character->id, sizeof(int), 8, file);
    // write the resource structs and attributes structs as ints
    fwrite(&character->base_resources, sizeof(int), 24, file);

    // writes the name and length
    const int name_length = (int) strlen(character->name) + 1;// +1 for '\0'
    fwrite(&name_length, sizeof(int), 1, file);
    fwrite(character->name, sizeof(char), name_length, file);

    // write the ability array data
    // write the ability count & allocated space
    fwrite(&character->abilities->ability_count, sizeof(int), 2, file);
    // iterate through the ability array and write the ids
    for (int i = 0; i < character->abilities->ability_count; i++) {
        fwrite(&character->abilities->abilities[i]->id, sizeof(int), 1, file);
    }
    return 0;
}

int read_character_data(FILE* file, character_t* character) {
    RETURN_WHEN_NULL(file, 1, "Character Save Handler", "In `read_character_data` given file is NULL")
    RETURN_WHEN_NULL(character, 1, "Character Save Handler", "In `read_character_data` given character is NULL")

    // read id, current_exp, needed_exp, level, has_map_key,
    // unspent_attr_p, unspent_res_p, max_carry_weight
    if (fread(&character->id, sizeof(int), 8, file) != 8) {
        destroy_character(character);
        log_msg(ERROR, "Save File Handler", "Failed to read character data");
        return 1;
    }
    // read the resource structs and attributes structs as ints
    if (fread(&character->base_resources, sizeof(int), 24, file) != 24) {
        destroy_character(character);
        log_msg(ERROR, "Save File Handler", "Failed to read character resources");
        return 1;
    }

    // read the name length
    int name_length;
    if (fread(&name_length, sizeof(int), 1, file) != 1) {
        destroy_character(character);
        log_msg(ERROR, "Save File Handler", "Failed to read character name length");
        return 1;
    }
    // allocate memory for the name
    character->name = malloc(name_length);
    if (character->name == NULL) {
        destroy_character(character);
        log_msg(ERROR, "Save File Handler", "Failed to allocate memory for character name");
        return 1;
    }
    // read the name
    if (fread(character->name, sizeof(char), name_length, file) != name_length) {
        destroy_character(character);
        log_msg(ERROR, "Save File Handler", "Failed to read character name");
        return 1;
    }

    // read the ability array data
    // read the ability count & allocated space
    int ability_count = 0;
    int allocated_space = 0;
    if (fread(&ability_count, sizeof(int), 1, file) != 1) {
        destroy_character(character);
        log_msg(ERROR, "Save File Handler", "Failed to read character ability count");
        return 1;
    }
    if (fread(&allocated_space, sizeof(int), 1, file) != 1) {
        destroy_character(character);
        log_msg(ERROR, "Save File Handler", "Failed to read character ability allocated space");
        return 1;
    }
    // initialize the ability array
    character->abilities = create_ability_array(allocated_space);
    if (character->abilities == NULL) {
        destroy_character(character);
        log_msg(ERROR, "Save File Handler", "Failed to allocate memory for character ability array");
        return 1;
    }
    // read the ability ids
    int ability_ids[ability_count];
    if (fread(ability_ids, sizeof(int), ability_count, file) != ability_count) {
        destroy_character(character);
        log_msg(ERROR, "Save File Handler", "Failed to read character ability ids");
        return 1;
    }
    // iterate through the ability ids and add them to the character
    for (int i = 0; i < ability_count; i++) {
        // add the ability to the character
        if (add_ability_c(character, ability_ids[i]) != 0) {
            destroy_character(character);
            log_msg(ERROR, "Save File Handler", "Failed to add ability to character");
            return 1;
        }
    }

    return 0;
}

long calculate_checksum_c(const character_t* character) {
    RETURN_WHEN_NULL(character, 0, "Character Save Handler",
                     "In `calculate_checksum_c` given character is NULL")

    long checksum = 0;

    checksum += character->id;
    checksum += character->current_exp;
    checksum += character->needed_exp;
    checksum += character->level;
    checksum += character->has_map_key;
    checksum += character->unspent_attr_p;
    checksum += character->unspent_res_p;
    checksum += character->max_carry_weight;
    // add resources to checksum
    checksum += character->base_resources.health;
    checksum += character->base_resources.stamina;
    checksum += character->base_resources.mana;
    checksum += character->max_resources.health;
    checksum += character->max_resources.stamina;
    checksum += character->max_resources.mana;
    checksum += character->current_resources.health;
    checksum += character->current_resources.stamina;
    checksum += character->current_resources.mana;
    // add attributes to checksum
    checksum += character->base_attributes.strength;
    checksum += character->base_attributes.intelligence;
    checksum += character->base_attributes.agility;
    checksum += character->base_attributes.endurance;
    checksum += character->base_attributes.luck;
    checksum += character->max_attributes.strength;
    checksum += character->max_attributes.intelligence;
    checksum += character->max_attributes.agility;
    checksum += character->max_attributes.endurance;
    checksum += character->max_attributes.luck;
    checksum += character->current_attributes.strength;
    checksum += character->current_attributes.intelligence;
    checksum += character->current_attributes.agility;
    checksum += character->current_attributes.endurance;
    checksum += character->current_attributes.luck;
    // add name length and name to checksum
    checksum += (long) strlen(character->name);
    for (int i = 0; i < strlen(character->name); i++) {
        checksum += character->name[i];
    }
    // add ability array data to checksum
    checksum += character->abilities->ability_count;
    checksum += character->abilities->allocated_space;
    for (int i = 0; i < character->abilities->ability_count; i++) {
        checksum += character->abilities->abilities[i]->id;
    }

    return checksum;
}
