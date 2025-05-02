#include "map_output.h"

#include "../../../../termbox2/termbox2.h"
#include "../../../logger/logger.h"
#include "../../colors.h"

void print_map(const vector2d_t anchor, const parsed_map_t* map) {
    RETURN_WHEN_NULL(map, , "Map Output", "Map is NULL");

    int anchor_x = anchor.dx;
    int anchor_y = anchor.dy;
    if (anchor.dx < 0 || anchor.dy < 0) {
        log_msg(ERROR, "Map Output", "Invalid anchor position: (%d, %d), set to (0, 0)", anchor.dx, anchor.dy);
        anchor_x = 0;
        anchor_y = 0;
    }

    for (int x = 0; x < map->width; x++) {
        for (int y = 0; y < map->height; y++) {
            const parsed_map_tile_t tile = map->tiles[x * map->height + y];
            tb_printf(anchor_x + x, anchor_y + y, tile.foreground_color, tile.background_color, "%c", tile.symbol);
        }
    }
}
