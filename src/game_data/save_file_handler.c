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
    #include <unistd.h>
    #include <sys/types.h>

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

int handler_checks(const char* save_name);

int ensure_save_dir(void);

int save_game_state(const char* save_name, const game_state_t* game_state) {
    if (handler_checks(save_name) != 0) return 1;

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
    const int name_length = (int) strlen(game_state->player->name) + 1; // +1 for '\0'
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
    if (handler_checks(save_name) != 0) return 1;

    char save_file_path[256];
    snprintf(save_file_path, sizeof(save_file_path), "%s%s%s", SAVE_FILE_DIR, PATH_SEP, save_name);

    FILE* file = fopen(save_file_path, "rb");
    RETURN_WHEN_NULL(file, 1, "Save File Handler", "Failed to open save file for writing");

    // reading the max floors and active map index from file
    FREAD(&game_state->max_floors, sizeof(int), 2, file, 1);
    for (int i = 0; i < game_state->max_floors; i++) {
        if (game_state->maps[i] == NULL){
            game_state->maps[i] = memory_pool_alloc(pool, sizeof(map_t));
            // read floor_nr, width, height, enemy_count, exit_unlocked,
            // entry_pos.dx, entry_pos.dy, exit_pos.dx, exit_pos.dy,
            // player_pos.dx and player_pos.dy
            FREAD(&game_state->maps[i]->floor_nr, sizeof(int), 11, file, 1);

            // read the hidden tiles
            game_state->maps[i]->hidden_tiles = memory_pool_alloc(pool, sizeof(map_tile_t) *
                                                                                game_state->maps[i]->width * game_state->maps[i]->height);
            FREAD(game_state->maps[i]->hidden_tiles, sizeof(map_tile_t),
                  game_state->maps[i]->width * game_state->maps[i]->height, file, 1);

            // read the revealed tiles
            game_state->maps[i]->revealed_tiles = memory_pool_alloc(pool, sizeof(map_tile_t) *
                                                                                  game_state->maps[i]->width * game_state->maps[i]->height);
            FREAD(game_state->maps[i]->revealed_tiles, sizeof(map_tile_t),
              game_state->maps[i]->width * game_state->maps[i]->height, file, 1);
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
    FREAD(&game_state->player->id, sizeof(int), 4, file, 1);
    // read the name
    int name_length;
    FREAD(&name_length, sizeof(int), 1, file, 1);
    game_state->player->name = malloc(name_length);
    FREAD(game_state->player->name, sizeof(char), name_length, file, 1);
    // read has_map_key, unspent_attr_p, unspent_res_p
    FREAD(&game_state->player->has_map_key, sizeof(int), 3, file, 1);
    // read ressource structs
    FREAD(&game_state->player->base_resources, sizeof(resources_t), 3, file, 1);
    // read attribute structs
    FREAD(&game_state->player->base_attributes, sizeof(attributes_t), 3, file, 1);
    // read the ability count
    FREAD(&game_state->player->ability_count, sizeof(int), 1, file, 1);
    // add each ability to the character
    for (int i = 0; i < game_state->player->ability_count; i++) {
        int ability_id;
        FREAD(&ability_id, sizeof(int), 1, file, 1);
        add_ability_c(game_state->player, ability_id);
    }
    
    fclose(file);

    return 0;
}

int get_save_files(char*** save_files, int* count) {
    // ensure the save directory exists
    if (ensure_save_dir() != 0) return 1;

    int file_count = 0;
    *count = 0;
    *save_files = NULL;

    #ifdef _WIN32
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    char search_path[MAX_PATH];

    snprintf(search_path, sizeof(search_path), "%s\\*", SAVE_FILE_DIR);

    hFind = FindFirstFileA(search_path, &findData);
    RETURN_WHEN_TRUE(hFind == INVALID_HANDLE_VALUE, 1, "Save File Handler", "Failed to find save files");

    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            file_count++;
        }
    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);

    *save_files = malloc(file_count * sizeof(char*));

    hFind = FindFirstFileA(search_path, &findData);
    RETURN_WHEN_TRUE(hFind == INVALID_HANDLE_VALUE, 1, "Save File Handler", "Failed to find save files");

    int index = 0;
    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            (*save_files)[index++] = _strdup(findData.cFileName);
        }
    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);

    #else //_WIN32
    // try to open the save directory
    DIR* dir = opendir(SAVE_FILE_DIR);
    RETURN_WHEN_NULL(dir, 1, "Save File Handler", "Failed to open save directory");

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            file_count++;
        }
    }

    rewinddir(dir);
    *save_files = malloc(file_count * sizeof(char*));

    int index = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            (*save_files)[index] = strdup(entry->d_name);
            index++;
        }
    }
    closedir(dir);

    #endif //else _WIN32

    *count = file_count;
    return 0;
}

int handler_checks(const char* save_name) {
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

