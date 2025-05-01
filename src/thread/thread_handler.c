#include "thread_handler.h"

#include <stdlib.h>

typedef struct {
    void (*func)(void);
} thread_func_wrapper_t;

#ifdef _WIN32
    #include <windows.h>

DWORD WINAPI thread_wrapper(LPVOID arg) {
    thread_func_wrapper_t* wrapper_arg = (thread_func_wrapper_t*) arg;
    wrapper_arg->func();
    free(wrapper_arg);
    return 0;
}

void start_simple_thread(void (*thread_func)(void)) {
    thread_func_wrapper_t* arg = malloc(sizeof(thread_func_wrapper_t));
    if (!arg) return;
    arg->func = thread_func;

    HANDLE thread = CreateThread(NULL, 0, thread_wrapper, arg, 0, NULL);
    if (thread) {
        CloseHandle(thread);// detach the thread
    } else {
        free(arg);
    }
}

#else
    #include <pthread.h>

void* thread_wrapper(void* arg) {
    thread_func_wrapper_t* wrapper_arg = (thread_func_wrapper_t*) arg;
    wrapper_arg->func();
    free(wrapper_arg);
    return NULL;
}

void start_simple_thread(void (*thread_func)(void)) {
    pthread_t thread;
    thread_func_wrapper_t* arg = malloc(sizeof(thread_func_wrapper_t));
    if (!arg) return;// Fehlerbehandlung
    arg->func = thread_func;

    if (pthread_create(&thread, NULL, thread_wrapper, arg) == 0) {
        pthread_detach(thread);// Detach den Thread
    } else {
        free(arg);// Fehlerbehandlung
    }
}
#endif
