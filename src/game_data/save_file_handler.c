#include "save_file_handler.h"

#include "../logger/logger.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <direct.h>

    #define STAT_STRUCT struct _stat
    #define STAT_FUNC _stat
    #define MKDIR(path) _mkdir(path)
    #define PATH_SEP "\\"
#else
    #include <dirent.h>

    #define STAT_STRUCT struct stat
    #define STAT_FUNC stat
    #define MKDIR(path) mkdir(path, 0755)
    #define PATH_SEP "/"
#endif

#define FREAD(ptr, size, count, stream, return_val) \
    if (fread(ptr, size, count, stream) != count) { \
        fclose(stream); \
        log_msg(ERROR, "Save File Handler", "Failed to read from save file"); \
        return return_val; \
    }

#define SAVE_FILE_DIR "save_files"

int div_checks(const char* save_name);

int ensure_save_dir(void);

int save_game_state(const char* save_name, const game_state_t* game_state) {
    if (div_checks(save_name) != 0) return 1;

    char save_file_path[256];
    snprintf(save_file_path, sizeof(save_file_path), "%s%s%s", SAVE_FILE_DIR, PATH_SEP, save_name);

    FILE* file = fopen(save_file_path, "wb");
    RETURN_WHEN_NULL(file, 1, "Save File Handler", "Failed to open save file for writing");

    // writing the max floors and active map index in file
    fwrite(&game_state->max_floors, sizeof(int), 2, file);
    for (int i = 0; i < game_state->max_floors; i++) {
        if (game_state->maps[i] != NULL) {
            // write floor_nr, width, height, enemy_count, exit_unlocked,
            // entry_pos.dx, entry_pos.dy, exit_pos.dx, exit_pos.dy,
            // player_pos.dx and player_pos.dy
            fwrite(&game_state->maps[i]->floor_nr, sizeof(int), 11, file);
            // write the hidden tiles
            fwrite(game_state->maps[i]->hidden_tiles, sizeof(map_tile_t),
                game_state->maps[i]->width * game_state->maps[i]->height, file);
            // write the revealed tiles
            fwrite(game_state->maps[i]->revealed_tiles, sizeof(map_tile_t),
                game_state->maps[i]->width * game_state->maps[i]->height, file);
        } else {
            log_msg(WARNING, "Save File Handler", "Map %d is NULL", i);
        }
    }

    //write character data
    // write id, current_exp, needed_exp, level
    fwrite(&game_state->player->id, sizeof(int), 4, file);
    //writes the name
    const int name_length = strlen(game_state->player->name) + 1; // +1 for '\0'
    fwrite(&name_length, sizeof(int), 1, file);
    fwrite(game_state->player->name, sizeof(char), name_length, file);
    // writes has_map_key, unspent_attr_p, unspent_res_p
    fwrite(&game_state->player->has_map_key, sizeof(int), 3, file);
    // writes ressource structs
    fwrite(&game_state->player->base_resources, sizeof(resources_t), 3, file);
    // writes attribute structs
    fwrite(&game_state->player->base_attributes, sizeof(attributes_t), 3, file);
    // write the ability count
    fwrite(&game_state->player->ability_count, sizeof(int), 1, file);
    // iterate through the abilities linked list and write only the ability ids
    const ability_node_t* current_node = game_state->player->abilities;
    while (current_node != NULL) {
        fwrite(&current_node->ability->id, sizeof(int), 1, file);
        current_node = current_node->next;
    }
    
    
    fclose(file);

    return 0;
}

int load_game_state(char* save_name, const memory_pool_t* pool, game_state_t* game_state) {
    if (div_checks(save_name) != 0) return 1;

    char save_file_path[256];
    snprintf(save_file_path, sizeof(save_file_path), "%s%s%s", SAVE_FILE_DIR, PATH_SEP, save_name);

    FILE* file = fopen(save_file_path, "rb");
    RETURN_WHEN_NULL(file, 1, "Save File Handler", "Failed to open save file for writing");

    // reading the max floors and active map index from file
    FREAD(&game_state->max_floors, sizeof(int), 2, file);
    for (int i = 0; i < game_state->max_floors; i++) {
        if (game_state->maps[i] == NULL){
            game_state->maps[i] = memory_pool_alloc(pool, sizeof(map_t));
            // read floor_nr, width, height, enemy_count, exit_unlocked,
            // entry_pos.dx, entry_pos.dy, exit_pos.dx, exit_pos.dy,
            // player_pos.dx and player_pos.dy
            FREAD(&game_state->maps[i]->floor_nr, sizeof(int), 11, file);

            // read the hidden tiles
            game_state->maps[i]->hidden_tiles = memory_pool_alloc(pool, sizeof(map_tile_t) *
                                                                                game_state->maps[i]->width * game_state->maps[i]->height);
            FREAD(game_state->maps[i]->hidden_tiles, sizeof(map_tile_t),
                  game_state->maps[i]->width * game_state->maps[i]->height, file);

            // read the revealed tiles
            game_state->maps[i]->revealed_tiles = memory_pool_alloc(pool, sizeof(map_tile_t) *
                                                                                  game_state->maps[i]->width * game_state->maps[i]->height);
            FREAD(game_state->maps[i]->revealed_tiles, sizeof(map_tile_t),
              game_state->maps[i]->width * game_state->maps[i]->height, file);
        } else {
            log_msg(ERROR, "Save File Handler", "Map %d is NULL", i);
            fclose(file);
            return 1;
        }
    }

    // malloc player
    game_state->player = memory_pool_alloc(pool, sizeof(character_t));

    // read character data
    // read id, current_exp, needed_exp, level
    FREAD(&game_state->player->id, sizeof(int), 4, file);
    // read the name
    int name_length;
    FREAD(&name_length, sizeof(int), 1, file);
    game_state->player->name = malloc(name_length);
    FREAD(game_state->player->name, sizeof(char), name_length, file, 1);
    // read has_map_key, unspent_attr_p, unspent_res_p
    FREAD(&game_state->player->has_map_key, sizeof(int), 3, file);
    // read ressource structs
    FREAD(&game_state->player->base_resources, sizeof(resources_t), 3, file);
    // read attribute structs
    FREAD(&game_state->player->base_attributes, sizeof(attributes_t), 3, file);
    // read the ability count
    FREAD(&game_state->player->ability_count, sizeof(int), 1, file);
    // add each ability to the character
    for (int i = 0; i < game_state->player->ability_count; i++) {
        int ability_id;
        FREAD(&ability_id, sizeof(int), 1, file);
        add_ability_c(game_state->player, ability_id);
    }
    
    fclose(file);

    return 0;
}

int div_checks(const char* save_name) {
    RETURN_WHEN_TRUE(ensure_save_dir() != 0, 1, "Save File Handler", "Failed to create save directory");
    const int save_name_length = (int) strlen(save_name);
    RETURN_WHEN_TRUE(save_name_length > 31, 1, "Save File Handler", "Save name is too long");
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

