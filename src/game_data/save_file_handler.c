#include "save_file_handler.h"

#include "../helper/string_helper.h"
#include "../logger/logger.h"
#include "character/character_save_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
    #define STAT_STRUCT struct _stat
    #define STAT_FUNC _stat
    #define MKDIR(path) _mkdir(path)
    #define PATH_SEP "\\"
#else
    #define STAT_STRUCT struct stat
    #define STAT_FUNC stat
    #define MKDIR(path) mkdir(path, 0755)
    #define PATH_SEP "/"
#endif

#define FREAD(ptr, size, count, stream, return_val)                           \
    if (fread(ptr, size, count, stream) != count) {                           \
        fclose(stream);                                                       \
        log_msg(ERROR, "Save File Handler", "Failed to read from save file"); \
        return return_val;                                                    \
    }

#define SAVE_FILE_DIR "save_files"

static struct {
    save_slot_t slot;
    const char* name;
} save_slot_files[] = {
        {SLOT_1, "save_file_1.sav"},
        {SLOT_2, "save_file_2.sav"},
        {SLOT_3, "save_file_3.sav"},
        {SLOT_4, "save_file_4.sav"},
        {SLOT_5, "save_file_5.sav"}};

/**
 * Performs necessary checks on the save file name to ensure validity.
 *
 * @param save_name The name of the save file to be checked.
 * @return Returns 0 if all checks pass, otherwise returns 1 indicating a failure in the checks.
 */
int save_file_checks(const char* save_name);

/**
 * Ensures that the save directory exists by creating it if it does not already exist.
 *
 * @return Returns 0 if the save directory exists or was successfully created.
 *         Returns 1 if an error occurred while attempting to create the directory.
 */
int ensure_save_dir(void);

/**
 * Calculates a checksum for the given game state to ensure data consistency.
 *
 * @param game_state A pointer to the game state structure to be processed.
 * @return Returns the calculated checksum as a long value.
 */
long calculate_checksum(const game_state_t* game_state);

/**
 * Allocates memory for an array of maps and initializes their associated resources using
 * the provided memory pool.
 *
 * @param pool Pointer to the memory pool to be used for memory allocation.
 * @param maps Double pointer to an array of map_t structures to be allocated and initialized.
 * @param length The number of maps to allocate and initialize.
 * @return Returns 0 if memory allocation and initialization are successful, otherwise returns 1
 *         if an error occurs (e.g., failed memory allocation).
 */
int allocate_maps(const memory_pool_t* pool, map_t** maps, int length);

/**
 * Sets the hidden_tiles and revealed_tiles pointers of all maps in the array to NULL.
 *
 * @param map An array of pointers to map_t structures, where the tile pointers should be initialized to NULL.
 * @param length The number of elements in the array.
 */
void set_maps_tiles_null(map_t** map, int length);

/**
 * Frees the allocated memory for an array of map structures and their associated resources.
 *
 * @param pool The memory pool used to allocate the map resources.
 * @param map A double pointer to the array of map_t structures to be freed.
 * @param length The number of map_t structures in the array.
 */
void free_map_resources(const memory_pool_t* pool, map_t** map, int length);

int save_game_state(const save_slot_t save_slot, const game_state_t* game_state) {
    RETURN_WHEN_TRUE(save_slot < 0 || save_slot >= MAX_SAVE_SLOTS, 1,
                     "Save File Handler", "In `save_game_state` given save slot %d is invalid", save_slot)

    char save_name[32];
    snprintf(save_name, sizeof(save_name), "%s", save_slot_files[save_slot].name);
    if (save_file_checks(save_name) != 0) return 1;

    char save_file_path[256];
    snprintf(save_file_path, sizeof(save_file_path), "%s%s%s", SAVE_FILE_DIR, PATH_SEP, save_name);

    FILE* file = fopen(save_file_path, "wb");
    RETURN_WHEN_NULL(file, 1, "Save File Handler", "Failed to open save file for writing")

    // write the iso timestamp length & string
    char timestamp[32];
    const size_t len = str_iso_time(timestamp, sizeof(timestamp)) + 1;// +1 for '\0'
    fwrite(&len, sizeof(int), 1, file);
    fwrite(timestamp, sizeof(char), len, file);

    // writing the max floors and active map index in file
    fwrite(&game_state->max_floors, sizeof(int), 2, file);

    // first write all the fixed integer values of each map
    for (int i = 0; i < game_state->max_floors; i++) {
        RETURN_WHEN_NULL(game_state->maps[i], 1, "Save File Handler", "In `save_game_state` given map %d is NULL", i)
        // write floor_nr, width, height, enemy_count, exit_unlocked,
        // entry_pos.dx, entry_pos.dy, exit_pos.dx, exit_pos.dy,
        // player_pos.dx and player_pos.dy
        fwrite(&game_state->maps[i]->floor_nr, sizeof(int), 11, file);
    }
    // then write the tiles of each map
    for (int i = 0; i < game_state->max_floors; i++) {
        const int map_size = game_state->maps[i]->width * game_state->maps[i]->height;
        // write the hidden tiles
        fwrite(game_state->maps[i]->hidden_tiles, sizeof(map_tile_t), map_size, file);
        // write the revealed tiles
        fwrite(game_state->maps[i]->revealed_tiles, sizeof(map_tile_t), map_size, file);
    }

    // write character data
    write_character_data(file, game_state->player);

    const long checksum = calculate_checksum(game_state);
    fwrite(&checksum, sizeof(long), 1, file);

    fclose(file);
    return 0;
}

int load_game_state(const save_slot_t save_slot, const memory_pool_t* pool, game_state_t* game_state) {
    RETURN_WHEN_TRUE(save_slot >= MAX_SAVE_SLOTS, 1,
                     "Save File Handler", "In `save_game_state` given save slot %d is invalid", save_slot)

    char save_name[32];
    snprintf(save_name, sizeof(save_name), "%s", save_slot_files[save_slot].name);
    if (save_file_checks(save_name) != 0) return 1;

    if (save_file_checks(save_name) != 0) return 1;

    char save_file_path[256];
    snprintf(save_file_path, sizeof(save_file_path), "%s%s%s", SAVE_FILE_DIR, PATH_SEP, save_name);

    FILE* file = fopen(save_file_path, "rb");
    RETURN_WHEN_NULL(file, 1, "Save File Handler", "Failed to open save file for writing")

    // read the iso timestamp length & string
    int len;
    char timestamp[32];
    FREAD(&len, sizeof(int), 1, file, 1)
    FREAD(timestamp, sizeof(char), len, file, 1)

    // reading the max floors and active map index from file
    FREAD(&game_state->max_floors, sizeof(int), 2, file, 1)
    // reading all the fixed integer values of each map
    for (int i = 0; i < game_state->max_floors; i++) {
        if (game_state->maps[i] != NULL) {
            log_msg(WARNING, "Save File Handler",
                    "Map %d is not NULL, the map pointer will be overwritten with a new allocated pointer.", i);
        }

        // allocate memory for the map
        game_state->maps[i] = memory_pool_alloc(pool, sizeof(map_t));

        // read floor_nr, width, height, enemy_count, exit_unlocked,
        // entry_pos.dx, entry_pos.dy, exit_pos.dx, exit_pos.dy,
        // player_pos.dx and player_pos.dy
        FREAD(&game_state->maps[i]->floor_nr, sizeof(int), 11, file, 1)
    }
    // allocate all the maps in the memory pool
    RETURN_WHEN_TRUE(allocate_maps(pool, game_state->maps, game_state->max_floors) != 0, 1,
                     "Save File Handler", "Failed to allocate memory for maps")

    // reading all the tiles of each map
    for (int i = 0; i < game_state->max_floors; i++) {
        const int width = game_state->maps[i]->width;
        const int height = game_state->maps[i]->height;
        const int map_size = width * height;

        // read the hidden tiles
        if (fread(game_state->maps[i]->hidden_tiles, sizeof(map_tile_t), map_size, file) != map_size) {
            free_map_resources(pool, game_state->maps, game_state->max_floors);
            fclose(file);
            log_msg(ERROR, "Save File Handler", "Failed to read hidden tiles");
            return 1;
        }
        // read the revealed tiles
        if (fread(game_state->maps[i]->revealed_tiles, sizeof(map_tile_t), map_size, file) != map_size) {
            free_map_resources(pool, game_state->maps, game_state->max_floors);
            fclose(file);
            log_msg(ERROR, "Save File Handler", "Failed to read revealed tiles");
            return 1;
        }
    }

    if (game_state->player != NULL) {
        log_msg(WARNING, "Save File Handler",
                "Player is not NULL, the player pointer will be overwritten with a new allocated pointer.");
    }
    // malloc player
    game_state->player = create_empty_character(0);
    if (game_state->player == NULL) {
        free_map_resources(pool, game_state->maps, game_state->max_floors);
        fclose(file);
        return 1;
    }
    // read character data
    if (read_character_data(file, game_state->player) != 0) {
        free_map_resources(pool, game_state->maps, game_state->max_floors);
        fclose(file);
        return 1;
    }

    // read the checksum
    long file_checksum;
    if (fread(&file_checksum, sizeof(long), 1, file) != 1) {
        destroy_character(game_state->player);
        game_state->player = NULL;
        free_map_resources(pool, game_state->maps, game_state->max_floors);
        fclose(file);
        log_msg(ERROR, "Save File Handler", "Failed to read checksum");
        return 1;
    }
    const long calculated_checksum = calculate_checksum(game_state);
    if (calculated_checksum != file_checksum) {
        log_msg(ERROR, "Save File Handler",
                "Checksum mismatch: expected %ld, got %ld", calculated_checksum, file_checksum);
    }

    fclose(file);
    return 0;
}

save_infos_t get_save_infos(void) {
    // ensure the save directory exists
    const save_infos_t empty = {NULL, 0};
    if (ensure_save_dir() != 0) return empty;

    save_infos_t save_infos;
    save_infos.dates = (char**) malloc(MAX_SAVE_SLOTS * sizeof(char*));
    RETURN_WHEN_NULL(save_infos.dates, empty, "Save File Handler", "Failed to allocate memory for save infos")
    save_infos.length = MAX_SAVE_SLOTS;

    for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
        save_infos.dates[i] = NULL;// pre-set the dates to NULL
    }

    // go through each save file, if the save file exists, read the length & timestamp string
    for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
        char save_file_path[1024];
        snprintf(save_file_path, sizeof(save_file_path), "%s%s%s", SAVE_FILE_DIR, PATH_SEP,
                 save_slot_files[i].name);

        STAT_STRUCT st;
        if (STAT_FUNC(save_file_path, &st) == 0) {
            // file exists
            FILE* file = fopen(save_file_path, "rb");
            if (file == NULL) {
                free(save_infos.dates);
                log_msg(ERROR, "Save File Handler", "Failed to open save file for reading");
                return empty;
            }
            // read the timestamp string length
            int timestamp_len;
            if (fread(&timestamp_len, sizeof(int), 1, file) != 1) {
                for (int j = 0; j < i; j++) {
                    if (save_infos.dates[j] != NULL) free(save_infos.dates[j]);
                }
                free(save_infos.dates);
                fclose(file);
                log_msg(ERROR, "Save File Handler", "Failed to read timestamp length");
                return empty;
            }
            // read the timestamp string
            save_infos.dates[i] = malloc(timestamp_len + 1);
            if (fread(save_infos.dates[i], sizeof(char), timestamp_len + 1, file) != timestamp_len + 1) {
                for (int j = 0; j <= i; j++) {
                    if (save_infos.dates[j] != NULL) free(save_infos.dates[j]);
                }
                free(save_infos.dates);
                fclose(file);
                log_msg(ERROR, "Save File Handler", "Failed to read timestamp string");
                return empty;
            }
            // always add the null terminator!
            save_infos.dates[i][timestamp_len] = '\0';
            fclose(file);// close the file
        }
    }
    return save_infos;
}

int save_file_checks(const char* save_name) {
    RETURN_WHEN_TRUE(ensure_save_dir() != 0, 1, "Save File Handler", "Failed to create save directory")
    const int save_name_length = (int) strlen(save_name);
    RETURN_WHEN_TRUE(save_name_length > 31, 1, "Save File Handler", "Save name is too long")
    return 0;
}

int ensure_save_dir(void) {
    STAT_STRUCT st;

    if (STAT_FUNC(SAVE_FILE_DIR, &st) == -1) {
        if (MKDIR(SAVE_FILE_DIR) == -1) {
            return 1;
        }
    }
    return 0;
}

long calculate_checksum(const game_state_t* game_state) {
    long checksum = 0;

    checksum += game_state->max_floors;
    checksum += game_state->active_map_index;

    for (int i = 0; i < game_state->max_floors; i++) {
        checksum += game_state->maps[i]->floor_nr;
        checksum += game_state->maps[i]->width;
        checksum += game_state->maps[i]->height;
        checksum += game_state->maps[i]->enemy_count;
        checksum += game_state->maps[i]->exit_unlocked;
        checksum += game_state->maps[i]->entry_pos.dx;
        checksum += game_state->maps[i]->entry_pos.dy;
        checksum += game_state->maps[i]->exit_pos.dx;
        checksum += game_state->maps[i]->exit_pos.dy;
        checksum += game_state->maps[i]->player_pos.dx;
        checksum += game_state->maps[i]->player_pos.dy;
        for (int j = 0; j < game_state->maps[i]->width * game_state->maps[i]->height; j++) {
            checksum += game_state->maps[i]->hidden_tiles[j];
            checksum += game_state->maps[i]->revealed_tiles[j];
        }
    }

    checksum += calculate_checksum_c(game_state->player);

    return checksum;
}

int allocate_maps(const memory_pool_t* pool, map_t** maps, const int length) {
    if (maps == NULL) return 1;
    if (pool == NULL) return 1;

    // check if the maps are allocated
    for (int i = 0; i < length; i++) {
        if (maps[i] == NULL) {
            log_msg(ERROR, "Save File Handler",
                    "Maps are not allocated, probably failed to allocate in `load_game_state`");
            for (int j = 0; j < length; j++) {
                // free all the previously allocated maps
                if (maps[j] != NULL) memory_pool_free(pool, maps[j]);
            }
            return 1;
        }
    }
    set_maps_tiles_null(maps, length);// pre-set all the tiles to NULL
    // allocate the hidden and revealed tiles
    for (int i = 0; i < length; i++) {
        maps[i]->hidden_tiles = memory_pool_alloc(pool, sizeof(map_tile_t) * maps[i]->width * maps[i]->height);
        maps[i]->revealed_tiles = memory_pool_alloc(pool, sizeof(map_tile_t) * maps[i]->width * maps[i]->height);

        if (maps[i]->hidden_tiles == NULL || maps[i]->revealed_tiles == NULL) {
            free_map_resources(pool, maps, i);
            log_msg(ERROR, "Save File Handler", "Failed to allocate memory for map tiles");
            return 1;
        }
    }
    return 0;
}

void set_maps_tiles_null(map_t** map, const int length) {
    if (map == NULL) return;
    for (int i = 0; i < length; i++) {
        if (map[i] != NULL) {
            map[i]->hidden_tiles = NULL;
            map[i]->revealed_tiles = NULL;
        }
    }
}

void free_map_resources(const memory_pool_t* pool, map_t** map, const int length) {
    if (map == NULL) return;
    for (int i = 0; i < length; i++) {
        if (map[i] != NULL) {
            if (map[i]->hidden_tiles != NULL) memory_pool_free(pool, map[i]->hidden_tiles);
            if (map[i]->revealed_tiles != NULL) memory_pool_free(pool, map[i]->revealed_tiles);
            memory_pool_free(pool, map[i]);
        }
    }
}
