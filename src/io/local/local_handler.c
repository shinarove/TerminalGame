#include "local_handler.h"

#include "../../logger/logger.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define PATH_SEP "\\"
#define LOCAL_DIRECTORY "ressources\\local"
#else
#define PATH_SEP "/"
#define LOCAL_DIRECTORY "ressources/local"
#endif//_WIN32

typedef struct observer_node observer_node_t;

typedef struct observer_node {
    void (*update_func)(void);
    observer_node_t* next;
} observer_node_t;

observer_node_t* observer_list = NULL;
FILE* local_file = NULL;
local_lang_t current_lang;

int init_local_handler(const local_lang_t lang) {
    if (local_file != NULL) {
        log_msg(WARNING, "Local", "Local handler is already initialized.");
        return 0;
    }

    current_lang = lang;

    char rel_path[128];
    snprintf(rel_path, sizeof(rel_path), "%s" PATH_SEP "%s", LOCAL_DIRECTORY, local_file_mapping[lang].file_name);

    local_file = fopen(rel_path, "r");
    RETURN_WHEN_NULL(local_file, 1, "Local", "Failed to open local file.");

    observer_list = malloc(sizeof(observer_node_t));
    RETURN_WHEN_NULL(observer_list, 1, "Local", "Failed to allocate memory for observer list.");

    observer_list->update_func = NULL;
    observer_list->next = NULL;
    return 0;
}

char* get_local_string(const char* key) {
    RETURN_WHEN_NULL(local_file, NULL, "Local", "Local handler is not initialized.");
    static char line[512];
    const size_t key_len = strlen(key);

    rewind(local_file);

    while (fgets(line, sizeof(line), local_file)) {
        //if the line is empty or a comment, skip it
        if (line[0] == '\n' || line[0] == '#') continue;

        if (line[key_len] == '=') {
            if (strncmp(line, key, key_len) == 0) {
                // get the starting position
                char* start = strchr(line, '"');
                if (!start) continue;

                // get the end position
                char* end = strchr(start + 1, '"');
                if (!end) continue;

                const size_t len = end - (start + 1);
                char* result = (char*) malloc(len + 1);
                RETURN_WHEN_NULL(result, NULL, "Local", "Failed to allocate memory for local string.");

                snprintf(result, len + 1, "%s", start + 1);
                return result;
            }
        }
    }

    return strdup(key);
}


int set_language(const local_lang_t lang) {
    RETURN_WHEN_NULL(local_file, 2, "Local", "Local handler is not initialized.");

    if (lang >= MAX_LANG) {
        log_msg(WARNING, "Local", "Invalid language: %d.", lang);
        return 2;
    }
    current_lang = lang;
    fclose(local_file);

    char rel_path[128];
    snprintf(rel_path, sizeof(rel_path), "%s" PATH_SEP "%s", LOCAL_DIRECTORY, local_file_mapping[lang].file_name);
    local_file = fopen(rel_path, "r");
    RETURN_WHEN_NULL(local_file, 1, "Local", "Failed to open local file.");

    // go through the observer list
    const observer_node_t* current = observer_list;
    while (current != NULL) {
        if (current->update_func != NULL) {
            current->update_func(); //call the observer function
        }
        current = current->next;
    }

    return 0;
}

local_lang_t get_language(void) {
    if (local_file == NULL) {
        log_msg(WARNING, "Local", "Local handler is not initialized.");
        return LANGE_EN; // default to English if not initialized
    }
    return current_lang;
}

void observe_local(void (*update_func)(void)) {
    RETURN_WHEN_NULL(local_file, , "Local", "Local handler is not initialized.");
    RETURN_WHEN_NULL(update_func, , "Local", "Invalid observer function.");

    observer_node_t* new_node = malloc(sizeof(observer_node_t));
    RETURN_WHEN_NULL(new_node, , "Local", "Failed to allocate memory for observer node.");

    new_node->update_func = update_func;
    new_node->next = NULL;
    observer_node_t* current = observer_list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

void shutdown_local_handler(void) {
    // free the observer list
    observer_node_t* current = observer_list;
    while (current != NULL) {
        observer_node_t* next = current->next;
        free(current);
        current = next;
    }

    if (local_file != NULL) fclose(local_file);
}
