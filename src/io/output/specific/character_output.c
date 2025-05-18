#include "character_output.h"

#include "../../../data_types/cache/string_cache.h"
#include "../../../logger/logger.h"
#include "../../local/local_handler.h"
#include "../../string_formats.h"
#include "../common/common_output.h"

#include <stdio.h>

#define NUM_CACHED_CHARS 3 // number of cached characters

typedef struct {
    character_t* character;
    int cache_index;
} character_cache_t;

enum cache_co_offset {
    NAME_LVL_STR,
    RES_HEALTH_STR,
    RES_STAMINA_STR,
    RES_MANA_STR,
    ATTR_STRENGTH_STR,
    ATTR_INTELLIGENCE_STR,
    ATTR_AGILITY_STR,
    ATTR_CONSTITUTION_STR,
    ATTR_LUCK_STR,
    MAX_CACHED_CO_STRINGS
};

enum co_index {
    LEVEL_STR,
    HEALTH_STR,
    STAMINA_STR,
    MANA_STR,
    STRENGTH_STR,
    INTELLIGENCE_STR,
    AGILITY_STR,
    CONSTITUTION_STR,
    LUCK_STR,
    MAX_CO_STRINGS
};

char** co_strings = NULL;

string_cache_t* character_cache = NULL;

void update_character_output_local(void);

char** prepare_char_strings(const character_t* character);

int init_character_output() {
    co_strings = (char**) malloc(sizeof(char*) * MAX_CO_STRINGS);
    RETURN_WHEN_NULL(co_strings, 1, "Character Output", "Failed to allocate memory for character output strings.")
    character_cache = create_string_cache(NUM_CACHED_CHARS, MAX_CACHED_CO_STRINGS);
    RETURN_WHEN_NULL(character_cache, 1, "Character Output", "Failed to create a string cache for character output.")

    update_character_output_local();
    observe_local(update_character_output_local);
    return 0;
}

void print_info_c(const int x, int y, const character_t* character, const int update) {
    RETURN_WHEN_NULL(co_strings, , "Character Output", "Module not initialized.")
    RETURN_WHEN_NULL(character, , "Character Output", "In `print_info_c` given player is NULL.")

    char** strings = get_strings_from_cache(character_cache, (void*) character);
    if (strings == NULL || update == 1) {
        // update the strings in the cache if they are not cached or if the update flag is set
        char** temp_strings = prepare_char_strings(character);
        RETURN_WHEN_NULL(temp_strings, , "Character Output", "In `print_info_c` failed to prepare strings.")

        strings = put_strings_in_cache(character_cache, (void*) character, temp_strings, MAX_CACHED_CO_STRINGS);
        RETURN_WHEN_NULL(strings, , "Character Output", "In `print_info_c` failed to put strings in cache.")
        free(temp_strings); // only the pointer to pointers needs to be freed, the rest is now cached
    }

    print_text(x, y++, WHITE, DEFAULT, strings[NAME_LVL_STR]);
    print_text_f(x, y++, WHITE, DEFAULT, RES_LINE_FORMAT_C, strings[RES_HEALTH_STR],
        strings[RES_STAMINA_STR], strings[RES_MANA_STR]);
    print_text_f(x, y, WHITE, DEFAULT, ATTR_LINE_FORMAT_C, strings[ATTR_STRENGTH_STR],
        strings[ATTR_INTELLIGENCE_STR], strings[ATTR_AGILITY_STR], strings[ATTR_CONSTITUTION_STR], strings[ATTR_LUCK_STR]);
}

void shutdown_character_output(void) {
    for (int i = 0; i < MAX_CO_STRINGS; i++) {
        if (co_strings[i] != NULL) {
            free(co_strings[i]);
        }
    }
    free(co_strings);
    co_strings = NULL;

    destroy_string_cache(character_cache);
    character_cache = NULL;
}

void update_character_output_local(void) {
    reset_string_cache(character_cache);

    for (int i = 0; i < MAX_CO_STRINGS; i++) {
        if (co_strings[i] != NULL) {
            free(co_strings[i]);
        }
    }
    co_strings[LEVEL_STR] = get_local_string("LEVEL");
    co_strings[HEALTH_STR] = get_local_string("HEALTH");
    co_strings[STAMINA_STR] = get_local_string("STAMINA");
    co_strings[MANA_STR] = get_local_string("MANA");
    co_strings[STRENGTH_STR] = get_local_string("STRENGTH");
    co_strings[INTELLIGENCE_STR] = get_local_string("INTELLIGENCE");
    co_strings[AGILITY_STR] = get_local_string("AGILITY");
    co_strings[CONSTITUTION_STR] = get_local_string("CONSTITUTION");
    co_strings[LUCK_STR] = get_local_string("LUCK");
}

char** prepare_char_strings(const character_t* character) {
    char** temp_strings = malloc(sizeof(char*) * MAX_CACHED_CO_STRINGS);
    if (temp_strings == NULL) return NULL;

    for (int i = 0; i < MAX_CACHED_CO_STRINGS; i++) {
        temp_strings[i] = malloc(sizeof(char) * 32);
        if (temp_strings[i] == NULL) return NULL;
    }

    // either use the character name or the local string (when enemy)
    char* character_name = character->id == 0 ? character->name : get_local_string(character->name);
    snprintf(temp_strings[NAME_LVL_STR], 32, NAME_LVL_FORMAT_C,
        character_name, co_strings[LEVEL_STR], character->level);
    free(character_name);

    // prepare the resource strings
    snprintf(temp_strings[RES_HEALTH_STR], 32, RESOURCE_FORMAT_C,
        co_strings[HEALTH_STR], character->current_resources.health, character->max_resources.health);
    snprintf(temp_strings[RES_STAMINA_STR], 32, RESOURCE_FORMAT_C,
        co_strings[STAMINA_STR], character->current_resources.stamina, character->max_resources.stamina);
    snprintf(temp_strings[RES_MANA_STR], 32, RESOURCE_FORMAT_C,
        co_strings[MANA_STR], character->current_resources.mana, character->max_resources.mana);

    // prepare the attribute strings
    snprintf(temp_strings[ATTR_STRENGTH_STR], 32, ATTRIBUTE_FORMAT_C,
        co_strings[STRENGTH_STR], character->current_attributes.strength);
    snprintf(temp_strings[ATTR_INTELLIGENCE_STR], 32, ATTRIBUTE_FORMAT_C,
        co_strings[INTELLIGENCE_STR], character->current_attributes.intelligence);
    snprintf(temp_strings[ATTR_AGILITY_STR], 32, ATTRIBUTE_FORMAT_C,
        co_strings[AGILITY_STR], character->current_attributes.agility);
    snprintf(temp_strings[ATTR_CONSTITUTION_STR], 32, ATTRIBUTE_FORMAT_C,
        co_strings[CONSTITUTION_STR], character->current_attributes.constitution);
    snprintf(temp_strings[ATTR_LUCK_STR], 32, ATTRIBUTE_FORMAT_C,
        co_strings[LUCK_STR], character->current_attributes.luck);

    return temp_strings;
}
