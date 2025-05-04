#include "character.h"

#include "../../logger/logger.h"

#include <stdio.h>

character_t* create_base_character(const memory_pool_t* pool, const int id, const char* name) {
    RETURN_WHEN_NULL(pool, NULL, "Character", "Pool is NULL");
    RETURN_WHEN_NULL(name, NULL, "Character", "Name is NULL");

    character_t* character = (character_t*) memory_pool_alloc(pool, sizeof(character_t));
    RETURN_WHEN_NULL(character, NULL, "Character", "Failed to allocate memory for character");

    character->id = id;
    snprintf(character->name, sizeof(character->name), "%s", name);

    const resources_t char_res = {10, 10, 10};
    character->base_resources = char_res;
    character->max_resources = char_res;
    character->current_resources = char_res;

    const attributes_t char_attr = {0, 0, 0, 0, 0};
    character->base_attributes = char_attr;
    character->max_attributes = char_attr;
    character->current_attributes = char_attr;

    return character;
}

character_t* create_character(const memory_pool_t* pool, const int id, const char* name, const resources_t base_res, const attributes_t base_attr) {
    RETURN_WHEN_NULL(pool, NULL, "Character", "Pool is NULL");
    RETURN_WHEN_NULL(name, NULL, "Character", "Name is NULL");

    character_t* character = (character_t*) memory_pool_alloc(pool, sizeof(character_t));

    character->id = id;
    snprintf(character->name, sizeof(character->name), "%s", name);

    character->base_resources = base_res;
    character->max_resources = base_res;
    character->current_resources = base_res;

    character->base_attributes = base_attr;
    character->max_attributes = base_attr;
    character->current_attributes = base_attr;

    return character;
}

void destroy_character(const memory_pool_t* pool, character_t* character) {
    memory_pool_free(pool, character);
}
