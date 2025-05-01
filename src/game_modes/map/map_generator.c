#include "map_generator.h"

#include "../../logger/logger.h"

#include <stdlib.h>

#define TOP 0
#define BOTTOM 1
#define LEFT 2
#define RIGHT 3

#define STANDARD_MAP_HEIGHT 19
#define STANDARD_MAP_WIDTH 39

void shuffle(vector2d_t* dir, const int n) {
    for (int i = n - 1; i > 0; i--) {
        const int j = rand() % (i + 1);
        const vector2d_t tmp = dir[j];
        dir[j] = dir[i];
        dir[i] = tmp;
    }
}

int generate_map(const memory_pool_t* pool, map_t* map_to_generate) {
    RETURN_WHEN_NULL(pool, 1, "Map Generator", "Memory pool is NULL");
    RETURN_WHEN_NULL(map_to_generate, 1, "Map Generator", "Map to generate is NULL");

    //check the size of the map
    if (map_to_generate->height <= 11 || map_to_generate->width <= 11) {
        log_msg(WARNING, "Map Generator", "Defined map dimensions are too small, using default dimensions");
        map_to_generate->height = STANDARD_MAP_HEIGHT;
        map_to_generate->width = STANDARD_MAP_WIDTH;
    } else if (map_to_generate->height % 2 == 0 || map_to_generate->width % 2 == 0) {
        log_msg(WARNING, "Map Generator", "Map dimensions are not odd, add 1 to each dimension");
        map_to_generate->height += 1;
        map_to_generate->width += 1;
    }
    //get the dimensions of the map for easier access
    const int width = map_to_generate->width;
    const int height = map_to_generate->height;

    //allocates memory for the maps
    map_to_generate->hidden_tiles = (map_tile_t*) memory_pool_alloc(pool, height * width * sizeof(map_tile_t));
    RETURN_WHEN_NULL(map_to_generate->hidden_tiles, 1, "Map Generator", "Failed to allocate memory for hidden tiles");
    map_to_generate->revealed_tiles = (map_tile_t*) memory_pool_alloc(pool, height * width * sizeof(map_tile_t));
    RETURN_WHEN_NULL(map_to_generate->revealed_tiles, 1, "Map Generator", "Failed to allocate memory for revealed tiles");

    //iterate through each tile and set it to WALL / HIDDEN
    for (int i = 0; i < height * width; i++) {
        map_to_generate->hidden_tiles[i] = WALL;
        map_to_generate->revealed_tiles[i] = HIDDEN;
    }
    //get random start edge
    const int start_edge = rand() % 4;

    //set the start position
    switch (start_edge) {
        case TOP:
            map_to_generate->player_pos.dx = 3 + 2 * (rand() % ((width - 5) / 2));
            map_to_generate->player_pos.dy = 1;
            map_to_generate->hidden_tiles[map_to_generate->player_pos.dx * height + 0] = START_DOOR;
            break;
        case BOTTOM:
            map_to_generate->player_pos.dx = 3 + 2 * (rand() % ((width - 5) / 2));
            map_to_generate->player_pos.dy = height - 2;
            map_to_generate->hidden_tiles[map_to_generate->player_pos.dx * height + (height - 1)] = START_DOOR;
            break;
        case LEFT:
            map_to_generate->player_pos.dx = 1;
            map_to_generate->player_pos.dy = 3 + 2 * (rand() % ((height - 5) / 2));
            map_to_generate->hidden_tiles[0 * height + map_to_generate->player_pos.dy] = START_DOOR;
            break;
        case RIGHT:
            map_to_generate->player_pos.dx = width - 2;
            map_to_generate->player_pos.dy = 3 + 2 * (rand() % ((height - 5) / 2));
            map_to_generate->hidden_tiles[(width - 1) * height + map_to_generate->player_pos.dy] = START_DOOR;
            break;
        default:
            log_msg(ERROR, "Map Generator", "Invalid start edge");
    }
    //initialize visited map copy for the dfs
    int visited[width][height];

    //check if the start position is valid for dfs (odd coordinates)
    if (map_to_generate->player_pos.dx % 2 == 0) {
        log_msg(WARNING, "Map Generator", "Player position X was even, should not be possible");
        map_to_generate->player_pos.dx += 1;
    } else if (map_to_generate->player_pos.dy % 2 == 0) {
        log_msg(WARNING, "Map Generator", "Player position Y was even, should not be possible");
        map_to_generate->player_pos.dy += 1;
    }
    //get the start position
    const int start_x = map_to_generate->player_pos.dx;
    const int start_y = map_to_generate->player_pos.dy;

    //prepare the stack for the dfs
    vector2d_t stack[width * height];
    int top = 0;

    //add start condition
    stack[top++] = (vector2d_t) {start_x, start_y};
    visited[start_x][start_y] = 1;
    map_to_generate->hidden_tiles[start_x * height + start_y] = FLOOR;

    //dfs loop
    while (top > 0) {
        const vector2d_t current = stack[top - 1];
        const int x = current.dx;
        const int y = current.dy;

        vector2d_t shuffled_dirs[4];
        for (int i = 0; i < 4; i++) {
            shuffled_dirs[i] = directions[i];
        }
        shuffle(shuffled_dirs, 4);

        int found = 0;

        for (int i = 0; i < 4; i++) {
            const int nx = x + shuffled_dirs[i].dx * 2;
            const int ny = y + shuffled_dirs[i].dy * 2;

            const bool is_in_bounds = nx >= 0 && nx < width && ny >= 0 && ny < height;
            const bool is_valid_cell = is_in_bounds && !visited[nx][ny];
            if (is_valid_cell) {
                const int wall_x = x + shuffled_dirs[i].dx;
                const int wall_y = y + shuffled_dirs[i].dy;

                map_to_generate->hidden_tiles[wall_x * height + wall_y] = FLOOR;
                map_to_generate->hidden_tiles[nx * height + ny] = FLOOR;
                visited[nx][ny] = 1;

                stack[top++] = (vector2d_t) {nx, ny};
                found = 1;
                break;
            }
        }

        if (!found) {
            top--; // backtracking
        }
    }

    const int num_loops = (width * height) / 100 + 1;

    int count = 0;
    int max_attempts = num_loops * 10;

    //add loops to the map
    while (count < num_loops && max_attempts > 0) {
        // Pick a random cell
        const int x = 1 + rand() % (width - 2);
        const int y = 1 + rand() % (height - 2);

        // If the wall has exactly 2 opposing floor neighbors, knock it down to create a loop
        if (map_to_generate->hidden_tiles[x * height + y] == WALL) {
            int neighbor_directions[4] = {0, 0, 0, 0};

            int floor_count = 0;
            for (int i = 0; i < 4; i++) {
                const int dx = x + directions[i].dx;
                const int dy = y + directions[i].dy;

                if (map_to_generate->hidden_tiles[dx * height + dy] == FLOOR) {
                    floor_count++;
                    neighbor_directions[i] = 1;
                }
            }

            // check if the wall has exactly 2 opposing floor neighbors
            if ((floor_count == 2) &&
                ((neighbor_directions[TOP] && neighbor_directions[BOTTOM]) ||
                 (neighbor_directions[LEFT] && neighbor_directions[RIGHT]))) {
                map_to_generate->hidden_tiles[x * height + y] = FLOOR;
                count++;
            }
        }

        max_attempts--;
    }

    int exit_x = 0;
    int exit_y = 0;
    // get a random exit edge that is different from the start edge
    int exit_edge = start_edge;
    while (exit_edge == start_edge) {
        exit_edge = rand() % 4;
    }

    bool valid_exit = false;
    do {
        switch (exit_edge) {
            case TOP:
                exit_x = 1 + 2 * (rand() % ((width - 2) / 2));
                exit_y = 0;
                valid_exit = map_to_generate->hidden_tiles[exit_x * height + exit_y + 1] == FLOOR;
                break;
            case BOTTOM:
                exit_x = 1 + 2 * (rand() % ((width - 2) / 2));
                exit_y = height - 1;
                valid_exit = map_to_generate->hidden_tiles[exit_x * height + exit_y - 1] == FLOOR;
                break;
            case LEFT:
                exit_x = 0;
                exit_y = 1 + 2 * (rand() % ((height - 2) / 2));
                valid_exit = map_to_generate->hidden_tiles[(exit_x + 1) * height + exit_y] == FLOOR;
                break;
            case RIGHT:
                exit_x = width - 1;
                exit_y = 1 + 2 * (rand() % ((height - 2) / 2));
                valid_exit = map_to_generate->hidden_tiles[(exit_x - 1) * height + exit_y] == FLOOR;
                break;
            default:
                log_msg(ERROR, "Map Generator", "Invalid exit edge");
                return 1;
        }
    } while (!valid_exit);

    map_to_generate->hidden_tiles[exit_x * height + exit_y] = EXIT_DOOR;

    return 0;
}
