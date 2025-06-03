#ifndef LIST_H
#define LIST_H

typedef struct {
    int (*add)(void* self, const void* element);
    int (*remove)(void* self, void* element);
    int (*remove_at)(void* self, int index);
    void* (*get)(const void* self, int index);
    int (*find)(const void* self, const void* element);
    int (*size)(const void* self);
    void (*clear)(void* self);
} List_VTable;


#endif //LIST_H
