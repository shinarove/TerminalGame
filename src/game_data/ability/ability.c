#include "ability.h"

#include "../../game_mechanics/dice/dice.h"
#include "../../helper/string_helper.h"
#include "../../io/local/local_handler.h"
#include "../../logger/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define PATH_SEP "\\"
    #define ABILITY_DIRECTORY "ressources\\game_data\\ability"
#else
    #define PATH_SEP "/"
    #define ABILITY_DIRECTORY "ressources/game_data/ability"
#endif//_WIN32

#define ABILITY_FILE_NAME "ability_table.csv"

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
                 "%s" PATH_SEP "%s", ABILITY_DIRECTORY, ABILITY_FILE_NAME);
        FILE* ability_file = fopen(rel_path, "r");

        char line[256];
        int count = -1;
        while (fgets(line, sizeof(line), ability_file) && count < MAX_ABILITIES) {
            if (count == -1) {
                count++;
                continue;// skip the header line
            }
            // read the id, convert it to int, and assign it to the ability
            const char* token = strtok(line, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read ability id at line %d", count + 1)
            RETURN_WHEN_TRUE(parse_int(token, &singleton_ability_table->abilities[count].id) != 0, NULL,
                             "Ability", "Failed to read ability id at line %d", count + 1)
            // check if the id is valid
            RETURN_WHEN_TRUE(singleton_ability_table->abilities[count].id != count,
                             NULL, "Ability", "Invalid ability id %d at line %d, should be %d.",
                             singleton_ability_table->abilities[count].id, count + 1, count)

            // read the name
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read ability name at line %d", count + 1)
            singleton_ability_table->abilities[count].key_name = strdup(token);
            singleton_ability_table->abilities[count].local_name = get_local_string(token);

            // read target character
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read c_target at line %d", count + 1)
            singleton_ability_table->abilities[count].c_target = token[0];
            // read target resource
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read r_target at line %d", count + 1)
            singleton_ability_table->abilities[count].r_target = token[0];
            // read effect type
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read effect type at line %d", count + 1)
            singleton_ability_table->abilities[count].effect_type = token[0];
            // read effect scaler
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read effect scaler at line %d", count + 1)
            singleton_ability_table->abilities[count].effect_scaler = token[0];
            // read accuracy scaler
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read accuracy scaler at line %d", count + 1)
            singleton_ability_table->abilities[count].accuracy_scaler = token[0];
            // read resource cost
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read r_cost at line %d", count + 1)
            singleton_ability_table->abilities[count].r_cost = token[0];

            int dice_size = 0;
            // read effect dice
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read ability effect dice at line %d", count + 1)
            RETURN_WHEN_TRUE(parse_int(token, &singleton_ability_table->abilities[count].effect_dice) != 0,
                             NULL, "Ability", "Failed to read ability effect dice at line %d", count + 1)
            dice_size = singleton_ability_table->abilities[count].effect_dice;
            RETURN_WHEN_TRUE(check_dice(dice_size) != 0,
                             NULL, "Ability", "Invalid effect dice %d at line %d", dice_size, count + 1)
            // read accuracy dice
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read accuracy dice at line %d", count + 1)
            RETURN_WHEN_TRUE(parse_int(token, &singleton_ability_table->abilities[count].accuracy_dice) != 0,
                             NULL, "Ability", "Failed to parse accuracy dice at line %d", count + 1)
            dice_size = singleton_ability_table->abilities[count].accuracy_dice;
            RETURN_WHEN_TRUE(check_dice(dice_size) != 0,
                             NULL, "Ability", "Invalid accuracy dice %d at line %d", dice_size, count + 1)
            // read effect rolls
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read  effect rolls at line %d", count + 1)
            RETURN_WHEN_TRUE(parse_int(token, &singleton_ability_table->abilities[count].effect_rolls) != 0,
                             NULL, "Ability", "Failed to parse effect rolls at line %d", count + 1)
            // read accuracy rolls
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read accuracy rolls at line %d", count + 1)
            RETURN_WHEN_TRUE(parse_int(token, &singleton_ability_table->abilities[count].accuracy_rolls) != 0,
                             NULL, "Ability", "Failed to parse accuracy rolls at line %d", count + 1)

            // read effect scale value
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read effect scale value at line %d", count + 1)
            RETURN_WHEN_TRUE(parse_float(token, &singleton_ability_table->abilities[count].effect_scale_value) != 0,
                             NULL, "Ability", "Failed to parse effect scale value at line %d", count + 1)
            // read accuracy scale value
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read accuracy scale value at line %d", count + 1)
            RETURN_WHEN_TRUE(parse_float(token, &singleton_ability_table->abilities[count].accuracy_scale_value) != 0,
                             NULL, "Ability", "Failed to parse accuracy scale value at line %d", count + 1)

            // read resource cost
            token = strtok(NULL, ",");
            RETURN_WHEN_NULL(token, NULL, "Ability", "Failed to read v_cost at line %d", count + 1)
            RETURN_WHEN_TRUE(parse_int(token, &singleton_ability_table->abilities[count].v_cost) != 0,
                             NULL, "Ability", "Failed to parse v_cost at line %d", count + 1)

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

void update_ability_local() {
    for (int i = 0; i < singleton_ability_table->count; i++) {
        free(singleton_ability_table->abilities[i].local_name);
        singleton_ability_table->abilities[i].local_name = get_local_string(singleton_ability_table->abilities[i].key_name);
    }
}
