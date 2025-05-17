#ifndef GOBLIN_H
#define GOBLIN_H

#include "stats.h"

constexpr attr_id_t lvl_table_goblin[] = {
        STRENGTH,
        STRENGTH,
        CONSTITUTION,
        AGILITY,
        STRENGTH,
        INTELLIGENCE,
        CONSTITUTION,
        AGILITY,
        AGILITY,
        LUCK,
        STRENGTH,
        INTELLIGENCE,
        CONSTITUTION,
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
        0, 100, 24, 28, 33,// TODO: changed the second exp value, to lvl up after a fight
        38, 44, 50, 56, 63,
        70, 77, 85, 93, 102,
        111, 121, 130, 141, 151, 162};

#endif//GOBLIN_H
