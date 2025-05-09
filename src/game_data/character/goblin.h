#ifndef GOBLIN_H
#define GOBLIN_H

#include "stats.h"

constexpr attr_id_t lvl_table_goblin[] = {
        STRENGTH,
        STRENGTH,
        ENDURANCE,
        AGILITY,
        STRENGTH,
        INTELLIGENCE,
        ENDURANCE,
        AGILITY,
        AGILITY,
        LUCK,
        STRENGTH,
        INTELLIGENCE,
        ENDURANCE,
        AGILITY,
        STRENGTH,
        STRENGTH,
        AGILITY,
        LUCK,
        INTELLIGENCE,
        STRENGTH};

/**
 * The table of the gained exp when defeating a goblin, with their respective level.
 */
static const int exp_table_goblin[] = {
        0, 100, 12, 14, 17,// TODO: change the first exp value, to lvl up after a fight
        19, 22, 25, 28, 31,
        35, 39, 43, 47, 51,
        56, 60, 65, 70, 76, 81};

#endif//GOBLIN_H
