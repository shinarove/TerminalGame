#include "map.h"

#include "../../logger/logger.h"

void destroy_map(const memory_pool_t* pool, map_t* map_to_destroy) {
    RETURN_WHEN_NULL(pool, , "Map", "Memory pool is NULL")
    RETURN_WHEN_NULL(map_to_destroy, , "Map", "Map to destroy is NULL")

    if (map_to_destroy->hidden_tiles != NULL) {
        memory_pool_free(pool, map_to_destroy->hidden_tiles);
        map_to_destroy->hidden_tiles = NULL;
    } else {
        log_msg(WARNING, "Map", "In `destroy_map` map to destroy has no hidden tiles");
    }
    if (map_to_destroy->revealed_tiles != NULL) {
        memory_pool_free(pool, map_to_destroy->revealed_tiles);
        map_to_destroy->revealed_tiles = NULL;
    } else {
        log_msg(WARNING, "Map", "In `destroy_map` map to destroy has no revealed tiles");
    }

    memory_pool_free(pool, map_to_destroy);
}
