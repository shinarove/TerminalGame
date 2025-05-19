#include "character_output.h"

#include "../../../data_types/cache/string_cache.h"
#include "../../../logger/logger.h"
#include "../../local/local_handler.h"
#include "../../string_formats.h"
#include "../common/common_output.h"
#include "../../../../termbox2/termbox2.h"

#include <stdio.h>

#define NUM_CACHED_CHARS 6// number of cached characters

typedef struct {
    character_t* character; // the character to be displayed
    output_args_c_t args; // the arguments for the character output
    int call_count; // the number of times this character with the given args has been called
} char_args_map_t;

enum char_str_cache_offset {
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

enum co_str_index {
    LEVEL_STR,
    HEALTH_STR,
    STAMINA_STR,
    MANA_STR,
    HEALTH_SHORT_STR,
    STAMINA_SHORT_STR,
    MANA_SHORT_STR,
    STRENGTH_STR,
    INTELLIGENCE_STR,
    AGILITY_STR,
    CONSTITUTION_STR,
    LUCK_STR,
    STRENGTH_SHORT_STR,
    INTELLIGENCE_SHORT_STR,
    AGILITY_SHORT_STR,
    CONSTITUTION_SHORT_STR,
    LUCK_SHORT_STR,
    MAX_CO_STRINGS
};

char** co_strings = NULL;

char_args_map_t** char_args_cache = NULL;
string_cache_t* char_str_cache = NULL;

void update_character_output_local(void);

char** prepare_char_strings(const character_t* character, output_args_c_t args);

char_args_map_t* wrap_char_args(character_t* character, output_args_c_t args);

int init_character_output() {
    co_strings = (char**) malloc(sizeof(char*) * MAX_CO_STRINGS);
    RETURN_WHEN_NULL(co_strings, 1, "Character Output", "Failed to allocate memory for character output strings.")
    char_args_cache = (char_args_map_t**) malloc(sizeof(char_args_map_t*) * NUM_CACHED_CHARS);
    RETURN_WHEN_NULL(char_args_cache, 1, "Character Output", "Failed to allocate memory for character args cache.")
    char_str_cache = create_string_cache(NUM_CACHED_CHARS, MAX_CACHED_CO_STRINGS);
    RETURN_WHEN_NULL(char_str_cache, 1, "Character Output", "Failed to create a string cache for character output.")

    for (int i = 0; i < MAX_CO_STRINGS; i++) {
        co_strings[i] = NULL;
    }
    for (int i = 0; i < NUM_CACHED_CHARS; i++) {
        char_args_cache[i] = NULL;
    }

    update_character_output_local();
    observe_local(update_character_output_local);
    return 0;
}

void print_c_res_attr_hori(const int x, int y, character_t* character, const output_args_c_t args) {
    RETURN_WHEN_NULL(co_strings, , "Character Output", "Module not initialized.")
    RETURN_WHEN_NULL(character, , "Character Output", "In `print_c_res_attr_hori` given player is NULL.")

    char_args_map_t* char_args = wrap_char_args(character, args); // wrap the character and args in a struct
    if (char_args == NULL) return;

    char** strings = get_strings_from_cache(char_str_cache, char_args);
    if (strings == NULL || character->u_flag_res == 1 || character->u_flag_attr == 1) {
        // update the strings in the cache if they are not cached or if the update flags are set
        char** temp_strings = prepare_char_strings(character, args);
        RETURN_WHEN_NULL(temp_strings, , "Character Output", "In `print_c_res_attr_hori` failed to prepare strings.")

        strings = put_strings_in_cache(char_str_cache, char_args, temp_strings, MAX_CACHED_CO_STRINGS);
        RETURN_WHEN_NULL(strings, , "Character Output", "In `print_c_res_attr_hori` failed to put strings in cache.")
        free(temp_strings);// only the pointer to pointers needs to be freed, the rest is now cached

        // reset the resource and attribute flags
        character->u_flag_res = 0;
        character->u_flag_attr = 0;
    }

    print_text(x, y++, WHITE, DEFAULT, strings[NAME_LVL_STR]);
    print_text_f(x, y++, WHITE, DEFAULT, RES_LINE_FORMAT_C, strings[RES_HEALTH_STR],
                 strings[RES_STAMINA_STR], strings[RES_MANA_STR]);
    print_text_f(x, y, WHITE, DEFAULT, ATTR_LINE_FORMAT_C, strings[ATTR_STRENGTH_STR],
                 strings[ATTR_INTELLIGENCE_STR], strings[ATTR_AGILITY_STR], strings[ATTR_CONSTITUTION_STR], strings[ATTR_LUCK_STR]);
}

void print_c_res_attr_vert(const int x, int y, character_t* character, const output_args_c_t args) {
    RETURN_WHEN_NULL(co_strings, , "Character Output", "Module not initialized.")
    RETURN_WHEN_NULL(character, , "Character Output", "In `print_c_res_attr_hori` given player is NULL.")

    char_args_map_t* char_args = wrap_char_args(character, args); // wrap the character and args in a struct
    if (char_args == NULL) return;

    char** strings = get_strings_from_cache(char_str_cache, char_args);
    if (strings == NULL || character->u_flag_res == 1 || character->u_flag_attr == 1) {
        // update the strings in the cache if they are not cached or if the update flag is set
        char** temp_strings = prepare_char_strings(character, args);
        RETURN_WHEN_NULL(temp_strings, , "Character Output", "In `print_c_res_attr_hori` failed to prepare strings.")

        strings = put_strings_in_cache(char_str_cache, char_args, temp_strings, MAX_CACHED_CO_STRINGS);
        RETURN_WHEN_NULL(strings, , "Character Output", "In `print_c_res_attr_hori` failed to put strings in cache.")
        free(temp_strings);// only the pointer to pointers needs to be freed, the rest is now cached

        // reset the resource and attribute flags
        character->u_flag_res = 0;
        character->u_flag_attr = 0;
    }

    print_text(x, y++, WHITE, DEFAULT, strings[NAME_LVL_STR]);
    print_text(x + 1, y++, WHITE, DEFAULT, strings[RES_HEALTH_STR]);
    print_text(x + 1, y++, WHITE, DEFAULT, strings[RES_STAMINA_STR]);
    print_text(x + 1, y++, WHITE, DEFAULT, strings[RES_MANA_STR]);
    y++;
    print_text(x + 1, y++, WHITE, DEFAULT, strings[ATTR_STRENGTH_STR]);
    print_text(x + 1, y++, WHITE, DEFAULT, strings[ATTR_INTELLIGENCE_STR]);
    print_text(x + 1, y++, WHITE, DEFAULT, strings[ATTR_AGILITY_STR]);
    print_text(x + 1, y++, WHITE, DEFAULT, strings[ATTR_CONSTITUTION_STR]);
    print_text(x + 1, y, WHITE, DEFAULT, strings[ATTR_LUCK_STR]);
}

void print_c_base_bonus_attr(const int x, int y, const character_t* character) {
    if (character == NULL) {
        log_msg(WARNING, "Character Output", "In `print_c_base_bonus_attr` given character is NULL");
        return;
    }

    // prints the two lines as follows:
    // Strength      | Intelligence      | Agility           | Constitution      | Luck
    // [5] +4        | [?] -?            | [?] +?            | [?] +?            | [?] -?

    const uintattr_t c_white = color_mapping[WHITE].value;
    const uintattr_t c_default = color_mapping[DEFAULT].value;
    const uintattr_t c_green = color_mapping[GREEN].value;
    const uintattr_t c_red = color_mapping[RED].value;

    tb_printf(x, y, c_white, c_default, "%s", co_strings[STRENGTH_STR]);
    tb_printf(x + 14, y, c_white, c_default, "| %s", co_strings[INTELLIGENCE_STR]);
    tb_printf(x + 34, y, c_white, c_default, "| %s", co_strings[AGILITY_STR]);
    tb_printf(x + 54, y, c_white, c_default, "| %s", co_strings[CONSTITUTION_STR]);
    tb_printf(x + 74, y++, c_white, c_default, "| %s", co_strings[LUCK_STR]);
    // new line
    // strength numbers
    tb_printf(x, y, c_white, c_default, "[%d]", character->base_attributes.strength);
    if (character->inventory->total_attribute_bonus.strength > 0) {
        tb_printf(x + 5, y, c_green, c_default, "+%d", character->inventory->total_attribute_bonus.strength);
    } else if (character->inventory->total_attribute_bonus.strength < 0) {
        tb_printf(x + 5, y, c_red, c_default, "%d", character->inventory->total_attribute_bonus.strength);
    } else {
        tb_printf(x + 5, y, c_white, c_default, "-");
    }
    // intelligence numbers
    tb_printf(x + 16, y, c_white, c_default, "[%d]", character->base_attributes.intelligence);
    if (character->inventory->total_attribute_bonus.intelligence > 0) {
        tb_printf(x + 21, y, c_green, c_default, "+%d", character->inventory->total_attribute_bonus.intelligence);
    } else if (character->inventory->total_attribute_bonus.intelligence < 0) {
        tb_printf(x + 21, y, c_red, c_default, "%d", character->inventory->total_attribute_bonus.intelligence);
    } else {
        tb_printf(x + 21, y, c_white, c_default, "-");
    }
    // agility numbers
    tb_printf(x + 36, y, c_white, c_default, "[%d]", character->base_attributes.agility);
    if (character->inventory->total_attribute_bonus.agility > 0) {
        tb_printf(x + 41, y, c_green, c_default, "+%d", character->inventory->total_attribute_bonus.agility);
    } else if (character->inventory->total_attribute_bonus.agility < 0) {
        tb_printf(x + 41, y, c_red, c_default, "%d", character->inventory->total_attribute_bonus.agility);
    } else {
        tb_printf(x + 41, y, c_white, c_default, "-");
    }
    // constitution numbers
    tb_printf(x + 56, y, c_white, c_default, "[%d]", character->base_attributes.constitution);
    if (character->inventory->total_attribute_bonus.constitution > 0) {
        tb_printf(x + 61, y, c_green, c_default, "+%d", character->inventory->total_attribute_bonus.constitution);
    } else if (character->inventory->total_attribute_bonus.constitution < 0) {
        tb_printf(x + 61, y, c_red, c_default, "%d", character->inventory->total_attribute_bonus.constitution);
    } else {
        tb_printf(x + 61, y, c_white, c_default, "-");
    }
    // luck numbers
    tb_printf(x + 76, y, c_white, c_default, "[%d]", character->base_attributes.luck);
    if (character->inventory->total_attribute_bonus.luck > 0) {
        tb_printf(x + 81, y, c_green, c_default, "+%d", character->inventory->total_attribute_bonus.luck);
    } else if (character->inventory->total_attribute_bonus.luck < 0) {
        tb_printf(x + 81, y, c_red, c_default, "%d", character->inventory->total_attribute_bonus.luck);
    } else {
        tb_printf(x + 81, y, c_white, c_default, "-");
    }

    tb_present();
}

void shutdown_character_output(void) {
    for (int i = 0; i < MAX_CO_STRINGS; i++) {
        if (co_strings[i] != NULL) {
            free(co_strings[i]);
        }
    }
    free(co_strings);
    co_strings = NULL;

    for (int i = 0; i < NUM_CACHED_CHARS; i++) {
        if (char_args_cache[i] != NULL) {
            free(char_args_cache[i]);
        }
    }
    free(char_args_cache);
    char_args_cache = NULL;

    destroy_string_cache(char_str_cache);
    char_str_cache = NULL;
}

void update_character_output_local(void) {
    reset_string_cache(char_str_cache);

    for (int i = 0; i < MAX_CO_STRINGS; i++) {
        if (co_strings[i] != NULL) {
            free(co_strings[i]);
        }
    }
    co_strings[LEVEL_STR] = get_local_string("LEVEL");

    co_strings[HEALTH_STR] = get_local_string("HEALTH");
    co_strings[STAMINA_STR] = get_local_string("STAMINA");
    co_strings[MANA_STR] = get_local_string("MANA");
    co_strings[HEALTH_SHORT_STR] = get_local_string("HEALTH.SHORT");
    co_strings[STAMINA_SHORT_STR] = get_local_string("STAMINA.SHORT");
    co_strings[MANA_SHORT_STR] = get_local_string("MANA.SHORT");

    co_strings[STRENGTH_STR] = get_local_string("STRENGTH");
    co_strings[INTELLIGENCE_STR] = get_local_string("INTELLIGENCE");
    co_strings[AGILITY_STR] = get_local_string("AGILITY");
    co_strings[CONSTITUTION_STR] = get_local_string("CONSTITUTION");
    co_strings[LUCK_STR] = get_local_string("LUCK");
    co_strings[STRENGTH_SHORT_STR] = get_local_string("STRENGTH.SHORT");
    co_strings[INTELLIGENCE_SHORT_STR] = get_local_string("INTELLIGENCE.SHORT");
    co_strings[AGILITY_SHORT_STR] = get_local_string("AGILITY.SHORT");
    co_strings[CONSTITUTION_SHORT_STR] = get_local_string("CONSTITUTION.SHORT");
    co_strings[LUCK_SHORT_STR] = get_local_string("LUCK.SHORT");
}

char** prepare_char_strings(const character_t* character, const output_args_c_t args) {
    char** temp_strings = malloc(sizeof(char*) * MAX_CACHED_CO_STRINGS);
    if (temp_strings == NULL) return NULL;

    for (int i = 0; i < MAX_CACHED_CO_STRINGS; i++) {
        temp_strings[i] = malloc(sizeof(char) * 32);
        if (temp_strings[i] == NULL) return NULL;
    }

    // either use the character name or the local string (when enemy)
    const bool player = character->id == 0;
    char* character_name = player ? character->name : get_local_string(character->name);
    snprintf(temp_strings[NAME_LVL_STR], 32, NAME_LVL_FORMAT_C,
             character_name, co_strings[LEVEL_STR], character->level);
    if (!player) free(character_name);

    // prepare offset for the resource strings
    int offset = args.res_attr_short == 0 ? HEALTH_STR : HEALTH_SHORT_STR;

    // prepare the resource strings
    if (args.res_curr_max == 1) {
        snprintf(temp_strings[RES_HEALTH_STR], 32, RES_CURR_MAX_FORMAT_C,
                co_strings[offset], character->current_resources.health, character->max_resources.health);
        snprintf(temp_strings[RES_STAMINA_STR], 32, RES_CURR_MAX_FORMAT_C,
                 co_strings[offset + 1], character->current_resources.stamina, character->max_resources.stamina);
        snprintf(temp_strings[RES_MANA_STR], 32, RES_CURR_MAX_FORMAT_C,
                 co_strings[offset + 2], character->current_resources.mana, character->max_resources.mana);
    } else {
        snprintf(temp_strings[RES_HEALTH_STR], 32, RES_CURR_FORMAT_C,
                co_strings[offset], character->base_resources.health);
        snprintf(temp_strings[RES_STAMINA_STR], 32, RES_CURR_FORMAT_C,
                 co_strings[offset + 1], character->base_resources.stamina);
        snprintf(temp_strings[RES_MANA_STR], 32, RES_CURR_FORMAT_C,
                 co_strings[offset + 2], character->base_resources.mana);
    }

    // prepare offset for the attribute strings
    offset = args.res_attr_short == 0 ? STRENGTH_STR : STRENGTH_SHORT_STR;

    // prepare the attribute strings
    if (args.attr_max == 1) {
        snprintf(temp_strings[ATTR_STRENGTH_STR], 32, ATTRIBUTE_FORMAT_C,
                co_strings[offset], character->max_attributes.strength);
        snprintf(temp_strings[ATTR_INTELLIGENCE_STR], 32, ATTRIBUTE_FORMAT_C,
                 co_strings[offset + 1], character->max_attributes.intelligence);
        snprintf(temp_strings[ATTR_AGILITY_STR], 32, ATTRIBUTE_FORMAT_C,
                 co_strings[offset + 2], character->max_attributes.agility);
        snprintf(temp_strings[ATTR_CONSTITUTION_STR], 32, ATTRIBUTE_FORMAT_C,
                 co_strings[offset + 3], character->max_attributes.constitution);
        snprintf(temp_strings[ATTR_LUCK_STR], 32, ATTRIBUTE_FORMAT_C,
                 co_strings[offset + 4], character->max_attributes.luck);
    } else {
        snprintf(temp_strings[ATTR_STRENGTH_STR], 32, ATTRIBUTE_FORMAT_C,
                co_strings[offset], character->base_attributes.strength);
        snprintf(temp_strings[ATTR_INTELLIGENCE_STR], 32, ATTRIBUTE_FORMAT_C,
                 co_strings[offset + 1], character->base_attributes.intelligence);
        snprintf(temp_strings[ATTR_AGILITY_STR], 32, ATTRIBUTE_FORMAT_C,
                 co_strings[offset + 2], character->base_attributes.agility);
        snprintf(temp_strings[ATTR_CONSTITUTION_STR], 32, ATTRIBUTE_FORMAT_C,
                 co_strings[offset + 3], character->base_attributes.constitution);
        snprintf(temp_strings[ATTR_LUCK_STR], 32, ATTRIBUTE_FORMAT_C,
                 co_strings[offset + 4], character->base_attributes.luck);
    }

    return temp_strings;
}

char_args_map_t* wrap_char_args(character_t* character, const output_args_c_t args) {
    // check if the character with the given args is already cached
    for (int i = 0; i < NUM_CACHED_CHARS; i++) {
        if (char_args_cache[i] != NULL) {
            if (char_args_cache[i]->character == character &&
                char_args_cache[i]->args.res_curr_max == args.res_curr_max &&
                char_args_cache[i]->args.attr_max == args.attr_max &&
                char_args_cache[i]->args.res_attr_short == args.res_attr_short) {

                char_args_cache[i]->call_count++;
                return char_args_cache[i];
            }
        }
    }
    // if not, create a new entry in the cache
    char_args_map_t* new_entry = malloc(sizeof(char_args_map_t));
    RETURN_WHEN_NULL(new_entry, NULL, "Character Output", "In `wrap_char_args` failed to allocate memory for new entry.")
    new_entry->character = character;
    new_entry->args = args;
    new_entry->call_count = 1;

    // find an empty slot in the cache
    int index = -1;
    for (int i = 0; i < NUM_CACHED_CHARS; i++) {
        if (char_args_cache[i] == NULL) {
            index = i;
            break;
        }
    }

    // if no empty slot is found, remove the least used entry
    if (index == -1) {
        int min_calls = char_args_cache[0]->call_count;
        index = 0;
        for (int i = 1; i < NUM_CACHED_CHARS; i++) {
            if (char_args_cache[i]->call_count < min_calls) {
                min_calls = char_args_cache[i]->call_count;
                index = i;
            }
        }
        free(char_args_cache[index]);
    }
    char_args_cache[index] = new_entry;
    return new_entry;
}
