#include "gear.h"

#include "../../helper/string_helper.h"
#include "../../io/local/local_handler.h"
#include "../../logger/logger.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #define PATH_SEP "\\"
    #define ABILITY_DIRECTORY "resources\\game_data\\gear"
#else
    #define PATH_SEP "/"
    #define ABILITY_DIRECTORY "resources/game_data/gear"
#endif//_WIN32

#define ABILITY_FILE_NAME "gear_table.csv"

#define INIT_ERROR_NULL(ptr, file, msg, ...)        \
    if (ptr == NULL) {                              \
        fclose(file);                               \
        log_msg(ERROR, "Gear", msg, ##__VA_ARGS__); \
        return NULL;                                \
    }

#define INIT_ERROR_TRUE(condition, file, msg, ...)  \
    if (condition) {                                \
        fclose(file);                               \
        log_msg(ERROR, "Gear", msg, ##__VA_ARGS__); \
        return NULL;                                \
    }

gear_table_t* singleton_gear_table = NULL;

void update_gear_local(void);

gear_table_t* init_gear_table(const memory_pool_t* pool) {
    RETURN_WHEN_NULL(pool, NULL, "Gear", "In `init_gear_table` given memory pool is NULL")

    if (singleton_gear_table == NULL) {
        // make singleton
        singleton_gear_table = (gear_table_t*) memory_pool_alloc(pool, sizeof(gear_table_t));
        singleton_gear_table->count = MAX_GEARS;

        char rel_path[64];
        snprintf(rel_path, sizeof(rel_path), "%s%s%s", ABILITY_DIRECTORY, PATH_SEP, ABILITY_FILE_NAME);
        FILE* gear_file = fopen(rel_path, "r");
        RETURN_WHEN_NULL(gear_file, NULL, "Gear", "In `init_gear_table` failed to open gear file %s", rel_path)

        char line[256];
        int count = -1;
        while (fgets(line, sizeof(line), gear_file) && count < MAX_GEARS) {
            if (count == -1) {
                count++;
                continue;// skip the header line
            }
            // read the id
            const char* token = strtok(line, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read gear id at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].id),
                            gear_file, "Failed to parse gear id at line %d", count + 1)
            // check if the id is valid
            INIT_ERROR_TRUE(singleton_gear_table->gears[count].id != count, gear_file,
                            "Invalid gear id %d at line %d should be %d.",
                            singleton_gear_table->gears[count].id, count + 1, count)

            // read the gear type
            int gear_type;
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read gear type at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &gear_type),
                            gear_file, "Failed to parse gear type at line %d", count + 1)
            INIT_ERROR_TRUE(gear_type < 0 || gear_type >= MAX_GEAR_TYPES, gear_file,
                            "Invalid gear type %d at line %d", gear_type, count + 1)
            singleton_gear_table->gears[count].gear_type = gear_type;

            // read the key name
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read gear key name at line %d", count + 1)
            singleton_gear_table->gears[count].key_name = strdup(token);
            singleton_gear_table->gears[count].local_name = get_local_string(token);

            // read the resource bonuses
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read resource bonuses (health) at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].resource_bonus.health),
                            gear_file, "Failed to parse resource bonuses (health) at line %d", count + 1)
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read resource bonuses (stamina) at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].resource_bonus.stamina),
                            gear_file, "Failed to parse resource bonuses (stamina) at line %d", count + 1)
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read resource bonuses (mana) at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].resource_bonus.mana),
                            gear_file, "Failed to parse resource bonuses (mana) at line %d", count + 1)

            // read the attribute bonuses
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read attribute bonuses (strength) at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].attribute_bonus.strength),
                            gear_file, "Failed to parse attribute bonuses (strength) at line %d", count + 1)
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read attribute bonuses (intelligence) at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].attribute_bonus.intelligence),
                            gear_file, "Failed to parse attribute bonuses (intelligence) at line %d", count + 1)
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read attribute bonuses (agility) at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].attribute_bonus.agility),
                            gear_file, "Failed to parse attribute bonuses (agility) at line %d", count + 1)
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read attribute bonuses (endurance) at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].attribute_bonus.endurance),
                            gear_file, "Failed to parse attribute bonuses (endurance) at line %d", count + 1)
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read attribute bonuses (luck) at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].attribute_bonus.luck),
                            gear_file, "Failed to parse attribute bonuses (luck) at line %d", count + 1)

            // read the ability count
            token = strtok(NULL, ",");
            INIT_ERROR_NULL(token, gear_file, "Failed to read ability count at line %d", count + 1)
            INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].ability_count),
                            gear_file, "Failed to parse ability count at line %d", count + 1)


            if (singleton_gear_table->gears[count].ability_count > 0) {
                // prepare the ability ids array
                singleton_gear_table->gears[count].ability_ids = memory_pool_alloc(pool,
                                                                                   sizeof(int) * singleton_gear_table->gears[count].ability_count);

                token = strtok(NULL, ",");
                INIT_ERROR_NULL(token, gear_file, "Failed to read ability ids at line %d", count + 1)
                if (singleton_gear_table->gears[count].ability_count == 1) {
                    // read single ability id
                    INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].ability_ids[0]),
                                    gear_file, "Failed to parse ability id at line %d", count + 1)

                } else {
                    // read the ability ids
                    char* ability_ids = strdup(token);
                    token = strtok(ability_ids, "-");
                    for (int i = 0; i < singleton_gear_table->gears[count].ability_count; i++) {
                        // read each ability id
                        INIT_ERROR_NULL(token, gear_file, "Failed to read ability ids at line %d", count + 1)
                        INIT_ERROR_TRUE(parse_int(token, &singleton_gear_table->gears[count].ability_ids[i]),
                                        gear_file, "Failed to parse ability id at line %d", count + 1)
                        // get the next token
                        token = strtok(NULL, "-");
                    }
                }
            }
            count++;
        }
        fclose(gear_file);
        observe_local(update_gear_local);
    }
    return singleton_gear_table;
}

gear_table_t* get_gear_table(void) {
    return singleton_gear_table;
}

void destroy_gear_table(const memory_pool_t* pool) {
    RETURN_WHEN_NULL(pool, , "Gear", "In `destroy_gear_table` given memory pool is NULL")

    for (int i = 0; i < singleton_gear_table->count; i++) {
        if (singleton_gear_table->gears[i].key_name != NULL) {
            free(singleton_gear_table->gears[i].key_name);
        }
        if (singleton_gear_table->gears[i].local_name != NULL) {
            free(singleton_gear_table->gears[i].local_name);
        }
        if (singleton_gear_table->gears[i].ability_ids != NULL) {
            memory_pool_free(pool, singleton_gear_table->gears[i].ability_ids);
        }
    }
    memory_pool_free(pool, singleton_gear_table);
    singleton_gear_table = NULL;
}

void update_gear_local(void) {
    for (int i = 0; i < singleton_gear_table->count; i++) {
        if (singleton_gear_table->gears[i].local_name != NULL) {
            free(singleton_gear_table->gears[i].local_name);
            singleton_gear_table->gears[i].local_name = get_local_string(singleton_gear_table->gears[i].key_name);
        }
    }
}
