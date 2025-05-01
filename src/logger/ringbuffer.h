#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#define BUFFER_SIZE 20

#include <pthread.h>

typedef struct {
    char** messages;
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ring_buffer_t;

/**
 * Initializes the ring buffer.
 *
 * @param buffer the buffer pointer to be initialized
 * @return 0 if initialization was successful or 1 if not
 */
int init_ringbuffer(ring_buffer_t* buffer);

/**
 * Frees the ring buffer.
 *
 * @param buffer the pointer to the ringbuffer
 */
void free_ringbuffer(const ring_buffer_t* buffer);

/**
 * Writes a message to the ring buffer.
 *
 * @param buffer the pointer to the ringbuffer
 * @param message the message to be written in the ringbuffer
 */
void write_to_ringbuffer(ring_buffer_t* buffer, const char* message);

/**
 * Reads a message from the ring buffer.
 *
 * @param buffer the pointer to the ringbuffer
 * @param message the placeholder of the message to be read
 * @return 0 if successful or non-zero when not
 */
int read_from_ringbuffer(ring_buffer_t* buffer, char* message);

#endif//RINGBUFFER_H
