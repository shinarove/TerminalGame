#ifndef MAP_H
#define MAP_H

#include "../../../termbox2/termbox2.h"

typedef enum {
    WALL,
    FLOOR,
    START_DOOR,
    EXIT_DOOR,
    DOOR_KEY,
    LIFE_FOUNTAIN,
    MANA_FOUNTAIN,
    STAMINA_FOUNTAIN,
    PLAYER,
    ENEMY,
    HIDDEN,
    MAX_MAP_TILES
} map_tile_t;

typedef struct {
    int dx;
    int dy;
} vector2d_t;

typedef struct {
    int id;
    int width;
    int height;
    int enemy_count;
    vector2d_t player_pos;
    map_tile_t* hidden_tiles; //the total size being height * width
    map_tile_t* revealed_tiles; //the total size being height * width
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
    uintattr_t foreground_color;
    uintattr_t background_color;
} tiles_mapping[] = {
    {WALL, '#', TB_BLUE, TB_BLUE},
    {FLOOR, ' ', TB_WHITE, TB_BLACK},
    {START_DOOR, '#', TB_GREEN, TB_BLACK},
    {EXIT_DOOR, '#', TB_YELLOW, TB_BLACK},
    {DOOR_KEY, '$', TB_YELLOW, TB_BLACK},
    {LIFE_FOUNTAIN, '+', TB_RED, TB_BLACK},
    {MANA_FOUNTAIN, '+', TB_BLUE, TB_BLACK},
    {STAMINA_FOUNTAIN, '+', TB_GREEN, TB_BLACK},
    {PLAYER, '@', TB_RED, TB_BLACK},
    {ENEMY, '!', TB_WHITE, TB_RED},
    {HIDDEN, ' ', TB_WHITE, TB_WHITE}
};

#endif //MAP_H
