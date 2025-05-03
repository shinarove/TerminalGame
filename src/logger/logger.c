#include "logger.h"

#include "../thread/thread_handler.h"
#include "logger_config.h"
#include "ringbuffer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
    #include <direct.h>

    #define STAT_STRUCT struct _stat
    #define STAT_FUNC _stat
    #define MKDIR(path) _mkdir(path)
    #define PATH_SEP "\\"
#else
    #include <dirent.h>

    #define STAT_STRUCT struct stat
    #define STAT_FUNC stat
    #define MKDIR(path) mkdir(path, 0755)
    #define PATH_SEP "/"
#endif

#define MAX_PATH_SIZE 4096

// relative directory from the project root
#define LOG_DIRECTORY "log"
#define LOG_FILE_FORMAT "log-%d.txt"

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define MSG_FORMAT "[%s] [%s] [%s] : %s\n"

const char* log_level_str[] = {"DEBUG", "FINE", "INFO", "WARNING", "ERROR"};

// === internal functions ===
/**
 * Ensures that the predefined log directory already exists,
 * if not create a new one.
 *
 * @return 0 if successfully created or already exists, 1 if failed
 */
int ensure_log_dir(void);

/**
 * Opens the log file with the current saved file id in appended modus.
 * If the file already exists, it will be first removed and then created.
 * If no file is found, create a new file corresponding to fopen(...).
 *
 * @param is_init if 0, no existing file will be removed
 * @return 0 if successfully opened, 1 if failed
 */
int open_log_file(int is_init);

/**
 * This function should be called whenever a new log msg must be written.
 *
 * Check if the log file is open, if not, a new file will be open.
 * - Either by creating a new file if no file is in the log directory.
 * - Or creating a new file if the last used file reaches the max size.
 *   The new file will get the current file id + 1 or 0, if the id reached the max number of files.
 * - Or opening the file that was last used, the current id will be set.
 * @return 0 if successfully, 1 if the current file was already closed, and a new file wasn't opened
 */
int check_log_file(void);

/**
 * This function gets the latest file id from the log directory.
 *
 * @return 0 or greater if successfully, -1 if failed
 * @note This function should only be called once when the logger is initialized.
 */
int get_latest_file_id(void);

/**
 * Starts the log writer thread.
 * This function will be called from the logger initialization function.
 */
void start_log_writer_thread(void);

/**
 * This function will be called from a different thread to read from the ringbuffer
 * and then write in the log file
 */
void log_writer_thread(void);

// === global variables ===
FILE* log_file = NULL;
ring_buffer_t log_buffer;

//states if the file writing thread is still running, if set to false, the thread terminates or is terminated
bool logger_is_running = false;
//the id of the used file
int file_id = 0;

int ensure_log_dir(void) {
    STAT_STRUCT st;

    if (STAT_FUNC(LOG_DIRECTORY, &st) == -1) {
        if (MKDIR(LOG_DIRECTORY) == -1) {
            return 1;
        }
    }
    return 0;
}

int open_log_file(const int is_init) {
    char name[16];
    snprintf(name, sizeof(name), LOG_FILE_FORMAT, file_id);

    char filename[256];
    snprintf(filename, sizeof(filename), "%s" PATH_SEP "%s", LOG_DIRECTORY, name);

    if (is_init) {
        // Check if the file already exists
        FILE* existing_file = fopen(filename, "r");
        if (existing_file) {
            fclose(existing_file);

            // If the file already exists, remove it
            if (remove(filename) != 0) {
                return 1;
            }
        }
    }

    log_file = fopen(filename, "a");
    if (!log_file) {
        return 1;
    }
    return 0;
}

int check_log_file(void) {
    if (log_file) {
        fseek(log_file, 0, SEEK_END);
        const long file_size = ftell(log_file);
        if (file_size >= MAX_FILE_SIZE) {
            fclose(log_file);//close the current file
            file_id = (file_id + 1) % MAX_N_FILES;

            // open a new file
            if (open_log_file(1) != 0) {
                // failed to open a new file
                return 1;
            }
        }
    }
    return 0;
}

int get_latest_file_id(void) {
    if (ensure_log_dir() != 0) {
        // failed to ensure the log directory
        return -1;
    }

    int latest_id = 0;
    DIR* dir = opendir(LOG_DIRECTORY);
    struct dirent* entry;
    time_t latest_time = 0;

    if (!dir) {
        // failed to open the log directory
        return -1;
    }

    // Iterate through the log directory and find the latest used file
    while ((entry = readdir(dir)) != NULL) {
        int id;
        struct stat file_stat;

        if (sscanf(entry->d_name, LOG_FILE_FORMAT, &id) == 1 && id >= 0 && id < MAX_N_FILES) {
            char filepath[MAX_PATH_SIZE];
            snprintf(filepath, sizeof(filepath), "%s/%s", LOG_DIRECTORY, entry->d_name);

            if (stat(filepath, &file_stat) == 0) {
                if (file_stat.st_mtime > latest_time) {
                    // found a newer file
                    latest_time = file_stat.st_mtime;
                    latest_id = id;
                }
            }
        }
    }

    closedir(dir);
    return latest_id;
}

void start_log_writer_thread(void) {
    logger_is_running = true;

    start_simple_thread(log_writer_thread);
}

void log_writer_thread() {
    bool running = true;
    while (running) {
        char log_msg[MAX_MSG_SIZE];
        if (read_from_ringbuffer(&log_buffer, log_msg) == 0) {
            // message successfully read from the ringbuffer
            check_log_file();
            if (log_file) {
                fprintf(log_file, "%s", log_msg);
                fflush(log_file);
            }
        }
        if (!logger_is_running) {
            // thread must be terminated
            running = false;
        }
    }
    //closes all pressures
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
    free_ringbuffer(&log_buffer);
}

void init_logger(void) {
    // ensures the init_logger can only be called when the file is null
    if (log_file == NULL) {
        if (init_ringbuffer(&log_buffer) == 0) {
            file_id = get_latest_file_id();
            if (file_id == -1 || open_log_file(0) != 0) {
                fclose(log_file);
            } else {
                start_log_writer_thread();
            }
        }
    }
}

void log_msg(const log_level_t level, const char* module, const char* format, ...) {
    if (log_file == NULL || !logger_is_running) {
        // logger is not initialized or not running
        return;
    }

    //get timestamp
    const time_t now = time(NULL);
    const struct tm* tm = localtime(&now);

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), TIMESTAMP_FORMAT, tm);

    const char* log_level;
    if (level >= MAX_LOG_LEVEL) {
        log_level = log_level_str[INFO];
    } else {
        log_level = log_level_str[level];
    }

    va_list args;
    va_start(args, format);
    char msg[MAX_MSG_SIZE - MAX_HEADER_SIZE];
    vsnprintf(msg, sizeof(msg), format, args);

    char log_msg[MAX_MSG_SIZE];
    snprintf(log_msg, MAX_MSG_SIZE, MSG_FORMAT, timestamp, log_level, module, msg);
    va_end(args);

    write_to_ringbuffer(&log_buffer, log_msg);
}

void shutdown_logger(void) {
    logger_is_running = false;
}
