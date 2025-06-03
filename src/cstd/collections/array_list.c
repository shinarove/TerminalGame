#include "array_list.h"

#include <string.h>

int array_list_add(void* self, const void* element);
int array_list_remove(void* self, const void* element);
int array_list_remove_at(void* self, int index);
void* array_list_get(const void* self, int index);
int array_list_find(const void* self, const void* element);
int array_list_size(const void* self);
void array_list_clear(void* self);

int array_list_reserve(ArrayList* self, int added_capacity);

static const List_VTable vtable_List = {
    .add = array_list_add,
    .remove = array_list_remove,
    .get = array_list_get,
    .find = array_list_find,
    .size = array_list_size,
    .clear = array_list_clear
};

static const ArrayList_VTable vtable_ArrayList = {
    .list = &vtable_List,
    .reserve = array_list_reserve
};

ArrayList* create_array_list(const size_t element_size, const unsigned int initial_capacity) {
    if (element_size == 0) return NULL; // Error: element size cannot be zero

    ArrayList* self = malloc(sizeof(ArrayList));
    if (self == NULL) return NULL; // Error: memory allocation failed

    self->allocated = element_size * initial_capacity;

    if (initial_capacity != 0) {
        self->elements = malloc(self->allocated);
        memset(self->elements, 0, self->allocated); // Initialize memory to zero
    } else {
        self->elements = NULL;
    }
    self->size = 0;
    self->element_size = element_size;

    self->vtable = &vtable_ArrayList;
    return self;
}

void destroy_array_list(ArrayList* list) {
    if (list == NULL) return;

    if (list->elements != NULL) {
        free(list->elements);
    }
    free(list);
}

int array_list_add(void* self, const void* element) {
    if (self == NULL || element == NULL) return -1; // Error: self or element is NULL

    ArrayList* list = self;
    const size_t space_filled = list->size * list->element_size;
    if (space_filled > list->allocated) return -1; // Error: invalid state encountered in list

    if (list->elements == NULL) {
        // if the elements pointer is NULL, we need to allocate memory for it
        list->elements = malloc(list->element_size);
        if (list->elements == NULL) return -1; // Error: memory allocation failed
        list->allocated = list->element_size;
        memset(list->elements, 0, list->allocated);

    } else if (space_filled == list->allocated) {
        const size_t new_capacity = list->allocated * 2; // double the allocated space
        void* new_elements = realloc(list->elements, new_capacity);
        if (new_elements == NULL) return -1; // Error: memory allocation failed

        // copy the old elements to the new memory location
        memcpy(new_elements, list->elements, list->allocated);

        list->elements = new_elements;
        list->allocated = new_capacity;
    }

    // copy content of element to the end of the list
    void* target = (char*)list->elements + space_filled;
    memcpy(target, element, list->element_size);

    list->size++;
    return 0; // Success
}

int array_list_remove(void* self, const void* element) {
    if (self == NULL || element == NULL) return -1; // Error: self or element is NULL

    ArrayList* list = self;
    for (int i = 0; i < list->size; i++) {
        void* current_element = (char*)list->elements + i * list->element_size;
        if (memcmp(current_element, element, list->element_size) == 0) {
            // Element found, remove it
            if (i < list->size - 1) {
                // Shift elements to the left
                const void* next_element = (char*) list->elements + ((i + 1) * list->element_size);
                const size_t bytes_to_move = (list->size - i - 1) * list->element_size;
                memmove(current_element, next_element, bytes_to_move);
            }
            list->size--;
            return 0; // Success
        }
    }
    return 1; // Element not found
}

int array_list_remove_at(void* self, const int index) {
    if (self == NULL) return -1; // Error: self is NULL

    ArrayList* list = self;
    if (index < 0 || index >= list->size) return -1; // Error: index out of bounds

    if (index < list->size - 1) {
        // Shift elements to the left
        const void* next_element = (char*) list->elements + ((index + 1) * list->element_size);
        const size_t bytes_to_move = (list->size - index - 1) * list->element_size;
        void* current_element = (char*)list->elements + index * list->element_size;
        memmove(current_element, next_element, bytes_to_move);
    }
    list->size--;
    return 0; // Success
}

void* array_list_get(const void* self, const int index) {
    if (self == NULL) return NULL; // Error: self is NULL

    const ArrayList* list = self;
    if (index < 0 || index >= list->size) return NULL; // Error: index out of bounds

    return (char*)list->elements + index * list->element_size; // Return pointer to the element at index
}

int array_list_find(const void* self, const void* element) {
    if (self == NULL || element == NULL) return -1; // Error: self or element is NULL

    const ArrayList* list = self;
    for (int i = 0; i < list->size; i++) {
        const void* current_element = (char*)list->elements + i * list->element_size;
        if (memcmp(current_element, element, list->element_size) == 0) {
            return i; // Element found at index i
        }
    }
    return 1; // Element not found
}

int array_list_size(const void* self) {
    if (self == NULL) return -1; // Error: self is NULL

    const ArrayList* list = self;
    return list->size; // Return the size of the list
}

void array_list_clear(void* self) {
    if (self == NULL) return; // Error: self is NULL

    ArrayList* list = self;
    list->size = 0; // Reset size to 0
}

int array_list_reserve(ArrayList* self, const int added_capacity) {
    if (self == NULL || added_capacity <= 0) return -1; // Error: self is NULL or added_capacity is negative or zero

    if (self->elements == NULL) {
        self->elements = malloc(added_capacity * self->element_size);
        if (self->elements == NULL) return -1; // Error: memory allocation failed
        self->allocated = added_capacity * self->element_size;

    } else if (self->allocated >= self->element_size * (self->size + added_capacity)) {
        return 0; // No need to reserve more space
    } else {
        const size_t new_capacity = self->allocated + added_capacity * self->element_size;
        void* new_elements = realloc(self->elements, new_capacity);
        if (new_elements == NULL) return -1; // Error: memory allocation failed

        // copy the old elements to the new memory location
        memcpy(new_elements, self->elements, self->allocated);

        self->elements = new_elements;
        self->allocated = new_capacity; // Update the allocated size
    }
    return 0; // Success
}
