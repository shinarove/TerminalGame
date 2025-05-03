#include "map_output.h"

#include "../../../../termbox2/termbox2.h"
#include "../../../logger/logger.h"
#include "../../colors.h"

void print_map(int x, int y, const parsed_map_t* map) {
    RETURN_WHEN_NULL(map, , "Map Output", "Map is NULL");

    if (x < 0 || y < 0) {
        log_msg(WARNING, "Map Output", "Invalid anchor position: (%d, %d), set to (0, 0)", x, y);
        x = 0;
        y = 0;
    }

    for (int i = 0; i < map->width; i++) {
        for (int j = 0; j < map->height; j++) {
            const parsed_map_tile_t tile = map->tiles[i * map->height + j];
            tb_printf(x + i, y + j, tile.foreground_color, tile.background_color, "%c", tile.symbol);
        }
    }

    tb_present();
}
