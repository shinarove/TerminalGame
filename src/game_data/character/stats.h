#ifndef STATS_H
#define STATS_H

typedef enum {
    HEALTH,
    STAMINA,
    MANA,
    MAX_RESOURCES
} res_id_t;

typedef enum {
    STRENGTH,
    INTELLIGENCE,
    AGILITY,
    ENDURANCE,
    LUCK,
    MAX_ATTRIBUTES
} attr_id_t;

typedef struct resources {
    unsigned short health;
    unsigned short stamina;
    unsigned short mana;
} resources_t;

typedef struct attributes {
    unsigned short strength;
    unsigned short intelligence;
    unsigned short agility;
    unsigned short endurance;
    unsigned short luck;
} attributes_t;

/**
 * The next needed experience points for each level from 0 to 20.
 * Lvl 0 & 20 have no experience points needed.
 *
 * The points are calculated using the formaula:
 * - x1 = 100
 * - xn = sqrt(xn-1) * 2 + xn-1
 */
static const int needed_exp_table[] = {
        0, 100, 120, 142, 166,
        191, 219, 249, 280, 314,
        349, 387, 426, 467, 510,
        556, 603, 652, 703, 756, 0};

#endif//STATS_H
