#ifndef MEM_MGMT_H
#define MEM_MGMT_H

#include <stdlib.h>

#define STANDARD_MEMORY_POOL_SIZE (8 * 1024 * 1024)        // 8MB
#define MIN_MEMORY_POOL_SIZE (1024 * 1024)                 // 1MB
#define MIN_MEMORY_BLOCK_SIZE (sizeof(memory_block_t) + 16)// 16 bytes for min user data

typedef struct memory_block_t {
    size_t size;                // size of the block (without the header)
    int active;                 // 1 if the block is in use, 0 if it is free
    struct memory_block_t* next;// pointer to the next block
    //here lays the user data
} memory_block_t;

typedef struct {
    size_t pool_size;     // size of the memory pool
    void* memory;         // the allocated memory pointer
    memory_block_t* first;// pointer to the first block
} memory_pool_t;

/**
 * Initialize a memory pool of the given size.
 * @param size the size of the memory pool to initialize,
 * when the given size is smaller than 1MB, the size will be automatically set to 1MB
 * @return The pointer to the memory pool. When NULL, the initialization failed
 */
memory_pool_t* init_memory_pool(size_t size);

/**
 * Allocates memory on the given memory pool.
 * If the remaining memory space is large enough, creates a new unused block for the remaining memory.
 *
 * @param pool the pool to allocate memory from
 * @param size the size of the memory to allocate
 * @return the pointer to the reserved memory space, or NULL if there is no free space on the pool
 */
void* memory_pool_alloc(const memory_pool_t* pool, size_t size);

/**
 * Sets the given data pointer to not active in the given memory pool.
 * But first checks if the pointer is contained in the memory pool.
 * If needed, it will defragment the memory pool and merge free blocks.
 *
 * @param pool the pool to free memory from
 * @param ptr the pointer to the memory to free
 */
void memory_pool_free(const memory_pool_t* pool, void* ptr);

/**
 * Frees the allocated memory pool.
 * @param pool the pool to free
 */
void shutdown_memory_pool(memory_pool_t* pool);

#endif//MEM_MGMT_H
