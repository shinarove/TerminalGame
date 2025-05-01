#ifndef THREAD_HANDLER_H
#define THREAD_HANDLER_H

/**
 * Starts a new thread with the given function.
 * The thread will be detached, so it will run independently.
 *
 * @param thread_func A simple function pointer to the function that will be executed in the thread.
 */
void start_simple_thread(void (*thread_func)(void));

#endif//THREAD_HANDLER_H
