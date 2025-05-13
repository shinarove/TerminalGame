#include "../../game_data/map/map_populator.h"

#include "../../logger/logger.h"

#include <stdlib.h>

#define STANDARD_ENEMY_COUNT 5

#define ENEMY_MIN_DISTANCE 3

/**
 * Place a key in the map at a specific location.
 * @param map_to_populate the map structure where the key will be placed
 */
void place_key(const map_t* map_to_populate);

/**
 * Place enemies randomly on the map based on the enemy count.
 * @param map_to_populate the map structure where enemies will be placed
 */
void place_enemy(map_t* map_to_populate);

/**
 * Place a fountain in the map at specific locations.
 * @param map_to_populate the map structure where the fountain(s) will be placed
 */
void place_fountain(const map_t* map_to_populate);

/**
 * Check if a cell is a dead end (only one neighbor)
 * @param x x coordinate of the cell
 * @param y y coordinate of the cell
 * @param map_to_check the map to check
 * @return 1 if it is a dead end, 0 otherwise
 */
int is_dead_end(int x, int y, const map_t* map_to_check);

/**
 * Checks if the specified position in the map is a floor tile.
 * @param x the x-coordinate of the position to check
 * @param y the y-coordinate of the position to check
 * @param map_to_check the map structure to examine
 * @return 1 if the tile at the specified position is a floor, 0 otherwise
 */
int is_not_floor(int x, int y, const map_t* map_to_check);

/**
 * Checks if the given coordinates are within a minimum distance to an enemy
 * or a start door on the map.
 * @param x the x-coordinate to check
 * @param y the y-coordinate to check
 * @param map_to_check the map structure to evaluate for nearby enemies or start doors
 * @return 1 if an enemy or a start door is within the minimum distance, 0 otherwise
 */
int is_close_to_enemy(int x, int y, const map_t* map_to_check);


int populate_map(map_t* map_to_populate) {
    RETURN_WHEN_NULL(map_to_populate, 1, "Map Populator", "Map to populate is NULL");
    RETURN_WHEN_NULL(map_to_populate->hidden_tiles, 1, "Map Populator", "Map to populate is not initialized");

    place_key(map_to_populate);
    place_enemy(map_to_populate);
    place_fountain(map_to_populate);

    return 0;
}

void place_key(const map_t* map_to_populate) {
    int x = 0;
    int y = 0;

    do {
        x = rand() % (map_to_populate->width - 2) + 1;
        y = rand() % (map_to_populate->height - 2) + 1;
    } while (is_not_floor(x, y, map_to_populate) && !is_dead_end(x, y, map_to_populate));

    map_to_populate->hidden_tiles[x * map_to_populate->height + y] = DOOR_KEY;
    DEBUG_LOG("Map Populator", "Key placed at %d, %d", x, y);
}

void place_enemy(map_t* map_to_populate) {
    //if the defined enemy count is smaller than 0, set it to the standard enemy count
    if (map_to_populate->enemy_count <= 0) {
        map_to_populate->enemy_count = STANDARD_ENEMY_COUNT;
    }

    for (int i = 0; i < map_to_populate->enemy_count; i++) {
        int x = 0;
        int y = 0;

        do {
            x = rand() % (map_to_populate->width - 2) + 1;
            y = rand() % (map_to_populate->height - 2) + 1;
        } while (is_not_floor(x, y, map_to_populate) || is_close_to_enemy(x, y, map_to_populate));

        map_to_populate->hidden_tiles[x * map_to_populate->height + y] = ENEMY;
    }
}

void place_fountain(const map_t* map_to_populate) {
    int x = 0;
    int y = 0;

    do {
        x = rand() % (map_to_populate->width - 2) + 1;
        y = rand() % (map_to_populate->height - 2) + 1;
    } while (is_not_floor(x, y, map_to_populate) && !is_dead_end(x, y, map_to_populate));

    map_to_populate->hidden_tiles[x * map_to_populate->height + y] = LIFE_FOUNTAIN;

    do {
        x = rand() % (map_to_populate->width - 2) + 1;
        y = rand() % (map_to_populate->height - 2) + 1;
    } while (is_not_floor(x, y, map_to_populate) && !is_dead_end(x, y, map_to_populate));

    map_to_populate->hidden_tiles[x * map_to_populate->height + y] = MANA_FOUNTAIN;
}

int is_dead_end(const int x, const int y, const map_t* map_to_check) {
    // Count neighboring non-wall cells
    int neighbor_count = 0;

    for (int i = 0; i < 4; i++) {
        const int dx = x + directions[i].dx;
        const int dy = y + directions[i].dy;

        const int map_idx = dx * map_to_check->height + dy;

        if (map_to_check->hidden_tiles[map_idx] != WALL) {
            neighbor_count++;
        }
    }

    return neighbor_count == 1;
}

int is_not_floor(const int x, const int y, const map_t* map_to_check) {
    return map_to_check->hidden_tiles[x * map_to_check->height + y] != FLOOR;
}

int is_close_to_enemy(const int x, const int y, const map_t* map_to_check) {
    for (int i = -ENEMY_MIN_DISTANCE; i <= ENEMY_MIN_DISTANCE + 1; i++) {
        for (int j = -ENEMY_MIN_DISTANCE; j <= ENEMY_MIN_DISTANCE + 1; j++) {
            const int map_idx = (x + i) * map_to_check->height + (y + j);
            if (map_to_check->hidden_tiles[map_idx] == ENEMY || map_to_check->hidden_tiles[map_idx] == START_DOOR) {
                return 1;
            }
        }
    }
    return 0;
}
