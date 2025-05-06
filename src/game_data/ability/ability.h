#ifndef ABILITY_H
#define ABILITY_H

#include "ability_index.h"
#include "../../memory/mem_mgmt.h"

typedef struct {
    int id;
    char* key_name;
    char* local_name;
} ability_t;

typedef struct {
    ability_t abilities[MAX_ABILITIES];
    unsigned int count;
} ability_table_t;

ability_table_t* init_ability_table(const memory_pool_t* pool);

ability_table_t* get_ability_table(void);

void destroy_ability_table(const memory_pool_t* pool);

#endif //ABILITY_H
