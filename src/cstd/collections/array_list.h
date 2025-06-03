#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "list.h"

#include <stdlib.h>

typedef struct ArrayList ArrayList;
typedef struct ArrayList_VTable ArrayList_VTable;

struct ArrayList{
    void* elements; // pointer to the array of elements
    int size; // number of elements in the list
    size_t element_size; // size of a single element
    size_t allocated; // the allocated space in memory in bytes
    const ArrayList_VTable* vtable;
};

struct ArrayList_VTable {
    const List_VTable* list;
    int (*reserve)(ArrayList* self, int added_capacity);
};

ArrayList* create_array_list(size_t element_size, unsigned int initial_capacity);

void destroy_array_list(ArrayList* list);

#endif //ARRAY_LIST_H
