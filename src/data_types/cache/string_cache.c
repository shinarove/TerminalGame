#include "string_cache.h"

#include "../../logger/logger.h"

#include <stdlib.h>

string_cache_t* create_string_cache(const int num_of_entities, const int num_of_str_per_entity) {
    RETURN_WHEN_TRUE(num_of_entities <= 0, NULL, "String Cache", "Number of entities must be greater than 0")
    RETURN_WHEN_TRUE(num_of_str_per_entity <= 0, NULL, "String Cache", "Number of strings per entity must be greater than 0")

    string_cache_t* cache = malloc(sizeof(string_cache_t));
    RETURN_WHEN_NULL(cache, NULL, "String Cache", "Failed to allocate memory for string cache")

    cache->num_of_entities = num_of_entities;
    cache->num_of_str_per_entity = num_of_str_per_entity;

    cache->cache_called = malloc(sizeof(int) * num_of_entities);
    RETURN_WHEN_NULL(cache->cache_called, NULL, "String Cache", "Failed to allocate memory for cached called array")

    cache->entities = malloc(sizeof(void*) * num_of_entities);
    RETURN_WHEN_NULL(cache->entities, NULL, "String Cache", "Failed to allocate memory for entities array")

    const int size = num_of_entities * num_of_str_per_entity;
    cache->strings = malloc(sizeof(char*) * size);
    RETURN_WHEN_NULL(cache->strings, NULL, "String Cache", "Failed to allocate memory for strings array")

    for (int i = 0; i < num_of_entities; i++) {
        cache->cache_called[i] = 0;
        cache->entities[i] = NULL;
    }
    for (int i = 0; i < size; i++) {
        cache->strings[i] = NULL;
    }

    return cache;
}

char** get_strings_from_cache(const string_cache_t* cache, const void* entity) {
    RETURN_WHEN_NULL(cache, NULL, "String Cache", "In `put_strings_in_cache` given cache is NULL")
    RETURN_WHEN_NULL(entity, NULL, "String Cache", "In `put_strings_in_cache` given entity is NULL")

    // check if the entity is already cached
    for (int i = 0; i < cache->num_of_entities; i++) {
        if (cache->entities[i] == entity) {
            cache->cache_called[i]++;
            return &cache->strings[i * cache->num_of_str_per_entity];
        }
    }
    return NULL;// entity is not cached
}

char** put_strings_in_cache(const string_cache_t* cache, void* entity, char** strings, const int num_of_strings) {
    RETURN_WHEN_NULL(cache, NULL, "String Cache", "In `put_strings_in_cache` given cache is NULL")
    RETURN_WHEN_NULL(entity, NULL, "String Cache", "In `put_strings_in_cache` given entity is NULL")
    RETURN_WHEN_NULL(strings, NULL, "String Cache", "In `put_strings_in_cache` given strings are NULL")
    RETURN_WHEN_TRUE(num_of_strings <= 0, NULL, "String Cache",
                     "In `put_strings_in_cache` number of strings must be greater than 0")

    if (num_of_strings > cache->num_of_str_per_entity) {
        log_msg(WARNING, "String Cache",
                "In `put_strings_in_cache` number of strings is greater than the cache size per entity");
    }
    const int max_iter = num_of_strings > cache->num_of_str_per_entity ? cache->num_of_str_per_entity : num_of_strings;

    // if the entity is already cached, update the strings and return the pointer
    for (int i = 0; i < cache->num_of_entities; i++) {
        if (cache->entities[i] == entity) {
            // update the strings
            for (int j = 0; j < max_iter; j++) {
                cache->strings[i * cache->num_of_str_per_entity + j] = strings[j];
            }
            cache->cache_called[i]++;
            return &cache->strings[i * cache->num_of_str_per_entity];
        }
    }

    // find an empty slot in the cache
    int index = -1;
    for (int i = 0; i < cache->num_of_entities; i++) {
        if (cache->entities[i] == NULL) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        // no empty slot found, find the least used entity
        int min_calls = cache->cache_called[0];
        index = 0;
        for (int i = 1; i < cache->num_of_entities; i++) {
            if (cache->cache_called[i] < min_calls) {
                min_calls = cache->cache_called[i];
                index = i;
            }
        }
    }

    // update the cache with the new entity and strings
    cache->entities[index] = entity;
    cache->cache_called[index] = 1;
    for (int j = 0; j < max_iter; j++) {
        cache->strings[index * cache->num_of_str_per_entity + j] = strings[j];
    }
    return &cache->strings[index * cache->num_of_str_per_entity];
}

int reset_entity_string_cache(const string_cache_t* cache, const void* entity) {
    RETURN_WHEN_NULL(cache, -1, "String Cache", "In `reset_entity_string_cache` given cache is NULL")
    RETURN_WHEN_NULL(entity, -1, "String Cache", "In `reset_entity_string_cache` given entity is NULL")

    for (int i = 0; i < cache->num_of_entities; i++) {
        if (cache->entities[i] == entity) {
            cache->cache_called[i] = 0;
            cache->entities[i] = NULL;
            for (int j = 0; j < cache->num_of_str_per_entity; j++) {
                free(cache->strings[i * cache->num_of_str_per_entity + j]);
                cache->strings[i * cache->num_of_str_per_entity + j] = NULL;
            }
            return 0;// entity was found and reset
        }
    }
    return 1;// entity was not found
}

void reset_string_cache(const string_cache_t* cache) {
    RETURN_WHEN_NULL(cache, , "String Cache", "In `reset_string_cache` given cache is NULL")

    for (int i = 0; i < cache->num_of_entities; i++) {
        cache->cache_called[i] = 0;
        cache->entities[i] = NULL;
        for (int j = 0; j < cache->num_of_str_per_entity; j++) {
            free(cache->strings[i * cache->num_of_str_per_entity + j]);
            cache->strings[i * cache->num_of_str_per_entity + j] = NULL;
        }
    }
}

void destroy_string_cache(string_cache_t* cache) {
    if (cache == NULL) return;

    for (int i = 0; i < cache->num_of_entities; i++) {
        for (int j = 0; j < cache->num_of_str_per_entity; j++) {
            free(cache->strings[i * cache->num_of_str_per_entity + j]);
        }
    }
    free(cache->cache_called);
    free(cache->entities);
    free(cache->strings);
    free(cache);
}
