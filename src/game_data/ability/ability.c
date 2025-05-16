#include "ability.h"

#include "../../game_mechanics/dice/dice.h"
#include "../../helper/string_helper.h"
#include "../../io/local/local_handler.h"
#include "../../logger/logger.h"
#include "../../game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define PATH_SEP "\\"
    #define ABILITY_DIRECTORY "resources\\game_data\\ability"
#else
    #define PATH_SEP "/"
    #define ABILITY_DIRECTORY "resources/game_data/ability"
#endif//_WIN32

#define ABILITY_FILE_NAME "ability_table.csv"

#define INIT_ERROR_NULL(ptr, file, msg, ...) \
    if (ptr == NULL) { \
        fclose(file); \
        log_msg(ERROR, "Ability", msg, ##__VA_ARGS__); \
        return NULL; \
    }

#define INIT_ERROR_TRUE(condition, file, msg, ...) \
    if (condition) { \
        fclose(file); \
        log_msg(ERROR, "Ability", msg, ##__VA_ARGS__); \
        return NULL; \
    }

ability_table_t* singleton_ability_table = NULL;

void update_ability_local(void);

ability_table_t* init_ability_table(const memory_pool_t* pool) {
    RETURN_WHEN_NULL(pool, NULL, "Ability", "Memory pool is NULL")

    if (singleton_ability_table == NULL) {
        //make a singleton
        singleton_ability_table = (ability_table_t*) memory_pool_alloc(pool, sizeof(ability_table_t));
        singleton_ability_table->count = MAX_ABILITIES;

        char rel_path[64];
        snprintf(rel_path, sizeof(rel_path),
                 "%s%s%s", ABILITY_DIRECTORY, PATH_SEP, ABILITY_FILE_NAME);
        FILE* ability_file = fopen(rel_path, "r");
        RETURN_WHEN_NULL(ability_file, NULL, "Ability", "In `init_ability_table` failed to open ability file %s", rel_path)

        char line[256];
        int count = -1;
        while (fgets(line, sizeof(line), ability_file) && count < MAX_ABILITIES) {
            if (count == -1) {
                count++;
                continue;// skip the header line
            }
            // read the id, convert it to int, and assign it to the ability
            const char* token = strtok(line, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read ability id at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_ability_table->abilities[count].id) != 0, ability_file,
                             "Failed to read ability id at line %d", count + 1)
            // check if the id is valid
            INIT_ERROR_TRUE(singleton_ability_table->abilities[count].id != count,
                             ability_file, "Invalid ability id %d at line %d, should be %d.",
                             singleton_ability_table->abilities[count].id, count + 1, count)

            // read the key name
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read ability name at line %d", count + 1)
            singleton_ability_table->abilities[count].key_name = strdup(token);
            singleton_ability_table->abilities[count].local_name = get_local_string(token);

            // read target character
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read c_target at line %d", count + 1)
            singleton_ability_table->abilities[count].c_target = token[0];
            // read target resource
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read r_target at line %d", count + 1)
            singleton_ability_table->abilities[count].r_target = token[0];
            // read effect type
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read effect type at line %d", count + 1)
            singleton_ability_table->abilities[count].effect_type = token[0];
            // read effect scaler
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read effect scaler at line %d", count + 1)
            singleton_ability_table->abilities[count].effect_scaler = token[0];
            // read accuracy scaler
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read accuracy scaler at line %d", count + 1)
            singleton_ability_table->abilities[count].accuracy_scaler = token[0];
            // read resource cost
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read r_cost at line %d", count + 1)
            singleton_ability_table->abilities[count].r_cost = token[0];

            int dice_size = 0;
            // read effect dice
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read ability effect dice at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_ability_table->abilities[count].effect_dice) != 0,
                             ability_file, "Failed to read ability effect dice at line %d", count + 1)
            dice_size = singleton_ability_table->abilities[count].effect_dice;
            INIT_ERROR_TRUE(check_dice(dice_size) != 0,
                             ability_file, "Invalid effect dice %d at line %d", dice_size, count + 1)
            // read accuracy dice
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read accuracy dice at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_ability_table->abilities[count].accuracy_dice) != 0,
                             ability_file, "Failed to parse accuracy dice at line %d", count + 1)
            dice_size = singleton_ability_table->abilities[count].accuracy_dice;
            INIT_ERROR_TRUE(check_dice(dice_size) != 0,
                             ability_file, "Invalid accuracy dice %d at line %d", dice_size, count + 1)
            // read effect rolls
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read  effect rolls at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_ability_table->abilities[count].effect_rolls) != 0,
                             ability_file, "Failed to parse effect rolls at line %d", count + 1)
            // read accuracy rolls
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read accuracy rolls at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_ability_table->abilities[count].accuracy_rolls) != 0,
                             ability_file, "Failed to parse accuracy rolls at line %d", count + 1)

            // read effect scale value
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read effect scale value at line %d", count + 1)
            INIT_ERROR_TRUE(parse_float(token, &singleton_ability_table->abilities[count].effect_scale_value) != 0,
                             ability_file, "Failed to parse effect scale value at line %d", count + 1)
            // read accuracy scale value
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read accuracy scale value at line %d", count + 1)
            INIT_ERROR_TRUE(parse_float(token, &singleton_ability_table->abilities[count].accuracy_scale_value) != 0,
                             ability_file, "Failed to parse accuracy scale value at line %d", count + 1)

            // read resource cost
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, ability_file, "Failed to read v_cost at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_ability_table->abilities[count].v_cost) != 0,
                             ability_file, "Failed to parse v_cost at line %d", count + 1)

            count++;
        }
        fclose(ability_file);
        observe_local(update_ability_local);
    }
    return singleton_ability_table;//always return the singleton
}

ability_table_t* get_ability_table() {
    return singleton_ability_table;
}

void destroy_ability_table(const memory_pool_t* pool) {
    RETURN_WHEN_NULL(pool, , "Ability", "Memory pool is NULL")

    for (int i = 0; i < singleton_ability_table->count; i++) {
        free(singleton_ability_table->abilities[i].key_name);
        free(singleton_ability_table->abilities[i].local_name);
    }

    memory_pool_free(pool, singleton_ability_table);
    singleton_ability_table = NULL;
}

ability_array_t* create_ability_array(const int pre_length) {
    RETURN_WHEN_TRUE(pre_length < 0, NULL, "Ability", "In `create_ability_array` allocated_space is negative")

    ability_array_t* ability_array = memory_pool_alloc(global_memory_pool, sizeof(ability_array_t));
    RETURN_WHEN_NULL(ability_array, NULL, "Ability", "In `create_ability_array` failed to allocate memory for ability array")

    if (pre_length == 0) {
        ability_array->abilities = NULL;
    } else {
        ability_array->abilities = (ability_t**) memory_pool_alloc(global_memory_pool, sizeof(ability_t*) * pre_length);
        RETURN_WHEN_NULL(ability_array->abilities, NULL, "Ability", "In `create_ability_array` failed to allocate memory for abilities")
        for (int i = 0; i < pre_length; i++) {
            ability_array->abilities[i] = NULL;
        }
    }
    ability_array->ability_count = 0;
    ability_array->allocated_space = pre_length;
    return ability_array;
}

int add_ability_a(ability_array_t* ability_array, const ability_id_t ability_id) {
    RETURN_WHEN_NULL(ability_array, 1, "Ability", "In `add_ability_a` given ability array is NULL")
    RETURN_WHEN_TRUE(ability_id < 0 || ability_id >= singleton_ability_table->count, 1,
                     "Ability", "In `add_ability_a` given ability id is invalid: %d", ability_id)
    RETURN_WHEN_TRUE(ability_array->ability_count > ability_array->allocated_space, 1,
                     "Ability", "In `add_ability_a` invalid state encountered,"
                                "the ability count is greater than the allocated space: %d > %d",
                     ability_array->ability_count, ability_array->allocated_space)

    if (ability_array->abilities == NULL || ability_array->allocated_space == 0) {
        // when the array is not allocated, allocate it with a default size of 5
        ability_array->abilities = (ability_t**) memory_pool_alloc(global_memory_pool, sizeof(ability_t*) * 5);
        for (int i = 0; i < 5; i++) {
            ability_array->abilities[i] = NULL;
        }
    } else if (ability_array->ability_count == ability_array->allocated_space) {
        // when the array is full, reallocate the array, with double the size
        ability_array->allocated_space *= 2;
        ability_array->abilities = (ability_t**) memory_pool_realloc(global_memory_pool, ability_array->abilities,
                                                                     sizeof(ability_t*) * ability_array->allocated_space);
        RETURN_WHEN_NULL(ability_array->abilities, 1, "Ability", "In `add_ability_a` failed to reallocate memory for abilities")
    }
    // add the ability to the array
    ability_array->abilities[ability_array->ability_count] = &get_ability_table()->abilities[ability_id];
    ability_array->ability_count++;

    return 0;
}

int remove_ability_a(ability_array_t* ability_array, const int index) {
    RETURN_WHEN_NULL(ability_array, 1, "Ability", "In `remove_ability_a` given ability array is NULL")
    RETURN_WHEN_TRUE(index < 0 || index >= ability_array->ability_count, 1,
                     "Ability", "In `remove_ability_a` given index is invalid: %d", index)

    // remove the ability from the array
    for (int i = index; i < ability_array->ability_count - 1; i++) {
        ability_array->abilities[i] = ability_array->abilities[i + 1];
    }
    ability_array->abilities[ability_array->ability_count - 1] = NULL;
    ability_array->ability_count--;

    return 0;
}

void destroy_ability_array(ability_array_t* ability_array) {
    RETURN_WHEN_NULL(ability_array, , "Ability", "In `destroy_ability_array` given ability array is NULL")

    if (ability_array->abilities != NULL) {
        memory_pool_free(global_memory_pool, ability_array->abilities);
    }
    memory_pool_free(global_memory_pool, ability_array);
}

void update_ability_local() {
    for (int i = 0; i < singleton_ability_table->count; i++) {
        free(singleton_ability_table->abilities[i].local_name);
        singleton_ability_table->abilities[i].local_name = get_local_string(singleton_ability_table->abilities[i].key_name);
    }
}
