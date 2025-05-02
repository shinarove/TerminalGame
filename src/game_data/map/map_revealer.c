#include "map_revealer.h"

#include "../../logger/logger.h"

static const vector2d_t check_vectors[4][2] = {
        {{1, 1}, {1, 0}},   // for up
        {{-1, -1}, {-1, 0}},// for down
        {{1, -1}, {0, -1}}, // for left
        {{-1, 1}, {0, 1}},  // for right
};

/**
 * Determines if the current iteration of a loop should be terminated based on the
 * positional relationship of the current tile and the previous wall encountered.
 *
 * @param x The x-coordinate of the current position on the map.
 * @param y The y-coordinate of the current position on the map.
 * @param dir The direction vector representing movement in the map.
 * @param j The current iteration index of the loop.
 * @param prev_wall_at Pointer to an integer that tracks the position of the most recently encountered wall.
 * @return 1 if the loop should be terminated; otherwise, 0.
 */
int need_loop_break(int x, int y, vector2d_t dir, int j, int* prev_wall_at);

int reveal_map(const map_t* map_to_reveal, const int light_radius) {
    RETURN_WHEN_NULL(map_to_reveal, 1, "Map Revealer", "Map to reveal is NULL");
    RETURN_WHEN_NULL(map_to_reveal->hidden_tiles, 1, "Map Revealer", "Map to reveal is not initialized");
    RETURN_WHEN_NULL(map_to_reveal->revealed_tiles, 1, "Map Revealer", "Revealed map is not initialized");
    RETURN_WHEN_TRUE(map_to_reveal->width <= 0, 1, "Map Revealer", "Width must be greater than 0");
    RETURN_WHEN_TRUE(map_to_reveal->height <= 0, 1, "Map Revealer", "Height must be greater than 0");

    if (light_radius <= 0) {
        return 0;
    }

    //get the map information, for easier access
    const int player_x = map_to_reveal->player_pos.dx;
    const int player_y = map_to_reveal->player_pos.dy;
    const int width = map_to_reveal->width;
    const int height = map_to_reveal->height;

    for (int i = 0; i < 4; i++) {
        const vector2d_t dir = directions[i];
        const vector2d_t diagonal_check = check_vectors[i][0];
        const vector2d_t reverse_check = check_vectors[i][1];

        int correction = 0;
        int prev_wall_at = -1;

        for (int j = 0; j <= light_radius; j++) {
            const int start_x = player_x + j * dir.dy;
            int start_y = player_y + j * dir.dx;

            if (dir.dx != 0) {
                //if the direction is vertical, negate the subtrahend
                start_y = player_y - j * dir.dx;
            }

            if (start_x < 0 || start_x >= width || start_y < 0 || start_y >= height) {
                //start position is out of bounds, skip to the next direction
                break;
            }
            for (int k = 1; k <= light_radius - correction; k++) {
                const int x = start_x + k * dir.dx;
                const int y = start_y + k * dir.dy;
                if (x < 0 || x >= width || y < 0 || y >= height) {
                    //calculated x or y is out of bound
                    break;
                }

                //calculated access index
                const int access_idx = x * height + y;

                if (map_to_reveal->revealed_tiles[access_idx] == HIDDEN) {
                    //initialize the relative diagonal and reverse tiles based on the y and x values
                    const int rel_diagonal = map_to_reveal->hidden_tiles[(x + diagonal_check.dx) * height + y + diagonal_check.dy];
                    const int rel_reverse = map_to_reveal->hidden_tiles[(x + reverse_check.dx) * height + y + reverse_check.dy];

                    if (rel_diagonal == WALL && rel_reverse == WALL && j > 1) {
                        //if the diagonal and reverse tiles are walls, and the distance from the player is greater than 1
                        //then the tile must be hidden because the reverse tile is blocking the view
                        break;
                    }

                    // if set to 1 break loop
                    int break_loop = 0;

                    switch (map_to_reveal->hidden_tiles[access_idx]) {
                        case WALL:
                            map_to_reveal->revealed_tiles[access_idx] = WALL;

                            if (need_loop_break(x, y, dir, j, &prev_wall_at)) {
                                break_loop = 1;
                            }
                            break;
                        case FLOOR:
                            map_to_reveal->revealed_tiles[access_idx] = FLOOR;
                            break;
                        case START_DOOR:
                            map_to_reveal->revealed_tiles[access_idx] = START_DOOR;
                            break;
                        case EXIT_DOOR:
                            map_to_reveal->revealed_tiles[access_idx] = EXIT_DOOR;
                            break;
                        case DOOR_KEY:
                            map_to_reveal->revealed_tiles[access_idx] = DOOR_KEY;
                            break;
                        case ENEMY:
                            map_to_reveal->revealed_tiles[access_idx] = ENEMY;
                            break;
                        case LIFE_FOUNTAIN:
                            map_to_reveal->revealed_tiles[access_idx] = LIFE_FOUNTAIN;
                            break;
                        case MANA_FOUNTAIN:
                            map_to_reveal->revealed_tiles[access_idx] = MANA_FOUNTAIN;
                            break;
                        case STAMINA_FOUNTAIN:
                            map_to_reveal->revealed_tiles[access_idx] = STAMINA_FOUNTAIN;
                            break;
                        default:
                            //does nothing
                            break;
                    }
                    if (break_loop) {
                        break;
                    }
                } else if (map_to_reveal->revealed_tiles[access_idx] == WALL && need_loop_break(x, y, dir, j, &prev_wall_at)) {
                    break;
                }
            }
            correction++;
        }
    }
    return 0;
}


int need_loop_break(const int x, const int y, const vector2d_t dir, const int j, int* prev_wall_at) {
    if (j == 0) {
        //gets the x or y value of the calculated coordinates
        *prev_wall_at = abs(y * dir.dy + x * dir.dx);
        return 1;
    }
    if (*prev_wall_at == abs(y * dir.dy + x * dir.dx)) {
        //if the previous j-loop had a wall at x or y coordinate, the loop must break
        //or else diagonal tiles (behind a wall and not visible by the player) are revealed
        return 1;
    }
    return 0;
}
