#ifndef STRING_CACHE_H
#define STRING_CACHE_H

typedef struct string_cache {
    int num_of_entities;
    int num_of_str_per_entity;

    int* cache_called;// an array of integers that counts how many times each entity is used
    void** entities;  // entities stored by reference
    char** strings;   // array of pointers to strings
} string_cache_t;

string_cache_t* create_string_cache(int num_of_entities, int num_of_str_per_entity);

char** get_strings_from_cache(const string_cache_t* cache, const void* entity);

char** put_strings_in_cache(const string_cache_t* cache, void* entity, char** strings, int num_of_strings);

int reset_entity_string_cache(const string_cache_t* cache, const void* entity);

void reset_string_cache(const string_cache_t* cache);

void destroy_string_cache(const string_cache_t* cache);

#endif//STRING_CACHE_H
