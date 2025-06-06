#include "character_save_handler.h"

#include "../../logger/logger.h"
#include "../ability/ability.h"

#include <string.h>

int write_character_data(FILE* file, const Character* character) {
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
    // write the ability count
    fwrite(&character->ability_list->size, sizeof(int), 1, file);
    // iterate through the ability array and write the ids
    for (int i = 0; i < character->ability_list->size; i++) {
        const ability_t* ability = character->vtable->get_ability_at(character, i);
        fwrite(&ability->id, sizeof(int), 1, file);
    }
    return 0;
}

int read_character_data(FILE* file, Character* character) {
    RETURN_WHEN_NULL(file, 1, "Character Save Handler", "In `read_character_data` given file is NULL")
    RETURN_WHEN_NULL(character, 1, "Character Save Handler", "In `read_character_data` given character is NULL")

    // read id, current_exp, needed_exp, level, has_map_key,
    // unspent_attr_p, unspent_res_p, max_carry_weight
    RETURN_WHEN_TRUE_CLEAN(fread(&character->id, sizeof(int), 8, file) != 8, 1,
                           destroy_character(character), "Save File Handler", "Failed to read character data")

    // read the resource structs and attributes structs as ints
    RETURN_WHEN_TRUE_CLEAN(fread(&character->base_resources, sizeof(int), 24, file) != 24, 1,
                           destroy_character(character), "Save File Handler", "Failed to read character resources")

    // read the name length
    int name_length;
    RETURN_WHEN_TRUE_CLEAN(fread(&name_length, sizeof(int), 1, file) != 1, 1,
                           destroy_character(character), "Save File Handler", "Failed to read character name length")

    // allocate memory for the name
    character->name = malloc(name_length);
    RETURN_WHEN_NULL_CLEAN(character->name, 1, destroy_character(character),
                           "Save File Handler", "Failed to allocate memory for character name");
    // read the name
    RETURN_WHEN_TRUE_CLEAN(fread(character->name, sizeof(char), name_length, file) != name_length, 1,
                           destroy_character(character), "Save File Handler", "Failed to read character name");

    // read the ability array data
    // read the ability count & allocated space
    int ability_count = 0;
    RETURN_WHEN_TRUE_CLEAN(fread(&ability_count, sizeof(int), 1, file) != 1, 1,
                           destroy_character(character), "Save File Handler", "Failed to read character ability count")

    // read the ability ids
    int ability_ids[ability_count];
    RETURN_WHEN_TRUE_CLEAN(fread(ability_ids, sizeof(int), ability_count, file) != ability_count, 1,
                           destroy_character(character), "Save File Handler", "Failed to read character ability ids");
    // iterate through the ability ids and add them to the character
    for (int i = 0; i < ability_count; i++) {
        // add the ability to the character
        const int add_success = character->vtable->add_ability(character, &get_ability_table()->abilities[i]);
        if (add_success == 1) {
            // ability already in the character's ability list
        } else if (add_success != 0) {
            destroy_character(character);
            log_msg(ERROR, "Save File Handler", "An error occurred when adding ability %d to character", ability_ids[i]);
            return 1;
        }
    }

    // TODO: read the inventory data

    return 0;
}

long calculate_checksum_c(const Character* character) {
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
    checksum += character->base_attributes.constitution;
    checksum += character->base_attributes.luck;
    checksum += character->max_attributes.strength;
    checksum += character->max_attributes.intelligence;
    checksum += character->max_attributes.agility;
    checksum += character->max_attributes.constitution;
    checksum += character->max_attributes.luck;
    checksum += character->current_attributes.strength;
    checksum += character->current_attributes.intelligence;
    checksum += character->current_attributes.agility;
    checksum += character->current_attributes.constitution;
    checksum += character->current_attributes.luck;
    // add name length and name to checksum
    checksum += (long) strlen(character->name);
    for (int i = 0; i < strlen(character->name); i++) {
        checksum += character->name[i];
    }
    // add ability array data to checksum
    checksum += character->ability_list->size;
    for (int i = 0; i < character->ability_list->size; i++) {
        const ability_t* ability = character->vtable->get_ability_at(character, i);
        checksum += ability->id;
    }

    return checksum;
}
