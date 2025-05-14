#ifndef ITEM_H
#define ITEM_H

typedef enum {
    HEAD_ARMOR,
    BODY_ARMOR,
    LEG_ARMOR,
    HAND_ARMOR,
    RING,
    AMULET,
    TOOL_ONE_HANDED,
    TOOL_TWO_HANDED,
    MAX_GEAR_TYPES
} gear_type_t;

typedef struct item {
    int id;
    gear_type_t gear_type;
    char* key_name;
    char* local_name;
} item_t;

#endif//ITEM_H
