#ifndef LOGGER_H
#define LOGGER_H

#define DEBUG_STATE 1//change to 0 to disable debug logging

#define DEBUG_LOG(modul, format, ...)                 \
    if (DEBUG_STATE == 1) {                           \
        log_msg(DEBUG, modul, format, ##__VA_ARGS__); \
    }

#define RETURN_WHEN_NULL(ptr, ret, modul, format, ...) \
    if (ptr == NULL) {                                 \
        log_msg(ERROR, modul, format, ##__VA_ARGS__);  \
        return ret;                                    \
    }

#define RETURN_WHEN_TRUE(expr, ret, modul, format, ...) \
    if (expr) {                                         \
        log_msg(ERROR, modul, format, ##__VA_ARGS__);   \
        return ret;                                     \
    }

typedef enum {
    DEBUG,
    FINE,
    INFO,
    WARNING,
    ERROR,
    MAX_LOG_LEVEL
} log_level_t;

/**
 * Initializes the logger and all related resources.
 */
void init_logger(void);

/**
 * Writes a log message to the log file. If the logger was not initialized, it will return immediately.
 *
 * @param level The log level of the message (DEBUG, FINE, INFO, WARNING, ERROR).
 * @param module The name of the module writing the log message.
 * @param format The format of the log message, similar to printf.
 * @param ... Additional arguments used in the format string.
 */
void log_msg(log_level_t level, const char* module, const char* format, ...);

/**
 * Shuts down the logger and all related resources.
 */
void shutdown_logger(void);
#endif//LOGGER_H
