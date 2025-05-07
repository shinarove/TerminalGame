#ifndef STATS_H
#define STATS_H

typedef enum {
    HEALTH,
    STAMINA,
    MANA
} res_identifier_t;

typedef enum {
    STRENGTH,
    INTELLIGENCE,
    AGILITY,
    ENDURANCE,
    LUCK
} attr_identifier_t;

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

#endif //STATS_H
