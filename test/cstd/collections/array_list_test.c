#include "../src/cstd/collections/array_list.h"

#include <stdio.h>
#include <assert.h>

ArrayList* test_create_array_list(const size_t element_size, const unsigned int initial_capacity) {
    ArrayList* list = create_array_list(element_size, initial_capacity);
    assert(list != NULL);
    assert(list->size == 0);
    assert(list->element_size == element_size);
    assert(list->allocated == element_size * initial_capacity);
    if (initial_capacity > 0) {
        assert(list->elements != NULL);
    } else {
        assert(list->elements == NULL);
    }
    printf("test_create_array_list: created array list with %u elements\n", list->size);
    return list;
}


void test_array_list_add(ArrayList** ptr_list) {
    ArrayList* list = *ptr_list;

    int* int_values = malloc(10 * sizeof(int));
    for (int i = 0; i < 10; i++) {
        int_values[i] = i + 1;
    }

    const int result = list->vtable->list->add(list, &int_values);
    assert(result == 0);
    assert(list->size == 1);

    free(int_values);
}

int main(void) {
    ArrayList* ptr_list = test_create_array_list(sizeof(int*), 5);

    test_array_list_add(&ptr_list);

    destroy_array_list(ptr_list);
    return 0;
}

