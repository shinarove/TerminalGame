#ifndef ABILITY_H
#define ABILITY_H

#include "../../memory/mem_mgmt.h"
#include "ability_index.h"

#define SELF_CHAR 's'
#define ENEMY_CHAR 'e'
#define HEALTH_CHAR 'h'
#define STAMINA_CHAR 's'
#define MANA_CHAR 'm'
#define ADDITIV_CHAR 'a'
#define SUBTRACTIVE_CHAR 's'
#define STRENGTH_CHAR 's'
#define INTELLIGENCE_CHAR 'i'
#define AGILITY_CHAR 'a'
#define ENDURANCE_CHAR 'e'
#define LUCK_CHAR 'l'

typedef struct ability {
    int id;
    char* key_name;
    char* local_name;
    char c_target;       // 's' for self, 'e' for enemy
    char r_target;       // 'h' for health, 's' for stamina, 'm' for mana
    char effect_type;    // 'd' for damage, 'h' for heal
    char effect_scaler;  // 's' for strength, 'i' for intelligence, 'a' for agility, 'e' for endurance, 'l' for luck
    char accuracy_scaler;// 's' for strength, 'i' for intelligence, 'a' for agility, 'e' for endurance, 'l' for luck
    char r_cost;         // 'h' for health, 's' for stamina, 'm' for mana

    int effect_dice;   // dice to calculate the base effect
    int accuracy_dice; // the used dice to determine if the user hits the target
    int effect_rolls;  // the number of rolls for the effect dice
    int accuracy_rolls;// the number of rolls for the accuracy dice

    float effect_scale_value;  // this value will be multiplied with the effect_scaler_attr and added to the effect_base_value
    float accuracy_scale_value;// this value will be multiplied with the accuracy_scaler_attr and added to the rolled accuracy value

    int v_cost;// the cost of the ability in the used resource (costs_resources)
} ability_t;

typedef struct {
    ability_t abilities[MAX_ABILITIES];
    unsigned int count;
} ability_table_t;

ability_table_t* init_ability_table(const memory_pool_t* pool);

ability_table_t* get_ability_table(void);

void destroy_ability_table(const memory_pool_t* pool);

#endif//ABILITY_H
