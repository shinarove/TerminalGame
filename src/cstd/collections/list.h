#ifndef LIST_H
#define LIST_H

typedef struct {
    /**
     * Adds an element to the list.
     * @param self Pointer to the list instance.
     * @param element Pointer to the element to be added.
     * @return 0 on success, -1 on error.
     */
    int (*add)(void* self, const void* element);
    /**
     * Removes an element from the list.
     * @param self Pointer to the list instance.
     * @param element Pointer to the element to be removed.
     * @return 0 on success, 1 on element not found, -1 on error.
     */
    int (*remove)(void* self, void* element);
    /**
     * Removes an element at a specific index from the list.
     * @param self Pointer to the list instance.
     * @param index The index of the element to be removed.
     * @return 0 on success, -1 on error.
     */
    int (*remove_at)(void* self, int index);
    /**
     * Retrieves an element at a specific index from the list.
     * @param self Pointer to the list instance.
     * @param index The index of the element to be retrieved.
     * @return Pointer to the element at the specified index, or NULL on error.
     */
    void* (*get)(const void* self, int index);
    /**
     * Finds an element in the list.
     * @param self Pointer to the list instance.
     * @param element Pointer to the element to be found.
     * @return The index of the element if found, or -1 on element not found, or -2 on error.
     */
    int (*find)(const void* self, const void* element);
    /**
     * Returns the size of the list.
     * @param self Pointer to the list instance.
     * @return The number of elements in the list, or -1 on error.
     */
    int (*size)(const void* self);
    /**
     * Clears the list, removing all elements.
     * @param self Pointer to the list instance.
     */
    void (*clear)(void* self);
} List_VTable;


#endif //LIST_H
