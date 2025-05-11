#ifndef MAP_H
#define MAP_H

#include "../../io/colors.h"

typedef enum {
    WALL,
    FLOOR,
    START_DOOR,
    EXIT_DOOR,
    DOOR_KEY,
    LIFE_FOUNTAIN,
    MANA_FOUNTAIN,
    STAMINA_FOUNTAIN,//currently unused
    PLAYER,
    ENEMY,
    HIDDEN,
    MAX_MAP_TILES
} map_tile_t;

typedef struct vector2d {
    int dx;
    int dy;
} vector2d_t;

typedef struct map {
    int floor_nr;// the floor-number this map represents
    int width;
    int height;
    int enemy_count;
    int exit_unlocked;   // 0 = no, 1 = yes
    vector2d_t entry_pos;// the entry position
    vector2d_t exit_pos; // the exit position
    vector2d_t player_pos;
    map_tile_t* hidden_tiles;  //the total size being height * width
    map_tile_t* revealed_tiles;//the total size being height * width
} map_t;

static const vector2d_t directions[4] = {
        {0, -1},// up
        {0, 1}, // down
        {-1, 0},// left
        {1, 0}  // right
};

static struct {
    map_tile_t tile;
    char symbol;
    color_t foreground_color;
    color_t background_color;
} tiles_mapping[] = {
        {WALL, '#', BLUE, BLUE},
        {FLOOR, ' ', WHITE, BLACK},
        {START_DOOR, '#', GREEN, BLACK},
        {EXIT_DOOR, '#', YELLOW, BLACK},
        {DOOR_KEY, '$', YELLOW, BLACK},
        {LIFE_FOUNTAIN, '+', RED, BLACK},
        {MANA_FOUNTAIN, '+', BLUE, BLACK},
        {STAMINA_FOUNTAIN, '+', GREEN, BLACK},
        {PLAYER, '@', RED, BLACK},
        {ENEMY, '!', WHITE, RED},
        {HIDDEN, ' ', WHITE, WHITE}};

#endif//MAP_H
