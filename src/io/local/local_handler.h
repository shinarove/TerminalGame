#ifndef LOCAL_HANDLER_H
#define LOCAL_HANDLER_H

typedef enum {
    LANGE_EN,
    LANGE_DE,
    MAX_LANG
} local_lang_t;

static const struct {
    local_lang_t lang;
    char* file_name;
} local_file_mapping[] = {
        {LANGE_EN, "local_en.properties"},
        {LANGE_DE, "local_de.properties"}};

/**
 * Initialize the local language handler by setting up the language and opening the corresponding resource file.
 *
 * This function initializes the handler to use the specified language. It opens the associated language file based
 * on a predefined mapping and prepares the observer list for updates. If the handler is already initialized, a warning
 * is logged and initialization is skipped.
 *
 * @param lang the language to be set for the local handler
 * @return 0 on success, or 1 if the resource file cannot be opened
 */
int init_local_handler(local_lang_t lang);

/**
 * Get the localized string for the given key.
 *
 * @param key the key for the localized string
 * @return the localized string, if the key is not found, returns the key itself
 * @note The caller must free the returned string.
 */
char* get_local_string(const char* key);

/**
 * Sets the current language for the local handler and updates all registered observers.
 *
 * This function updates the current language setting which determines the active language
 * file to be used. It attempts to close the current language file, opens the new associated
 * language resource file, and subsequently notifies the observer list by invoking their
 * respective update functions. The function ensures the validity of the input language and
 * handles errors related to file operations or uninitialized states.
 *
 * @param lang the language to be set (e.g., LANGE_EN, LANGE_DE)
 * @return 0 on success, 1 if the specified language file cannot be opened, or 2 if the handler
 *         is not initialized or the language is invalid
 */
int set_language(local_lang_t lang);


/**
 * Retrieve the currently set language of the local handler.
 *
 * This function returns the current language configured for the local handler.
 * If the local handler is not initialized, it logs a warning message and defaults the language to English.
 *
 * @return the current language as a value of local_lang_t, or LANGE_EN if the handler is not initialized
 */
local_lang_t get_language(void);

/**
 * Registers an observer function to be notified of updates from the local handler.
 *
 * This function adds a new observer to the observer list maintained by the local handler. The observer
 * function will be invoked when relevant updates occur. The observer handler must be initialized before
 * calling this function. If the observer function or handler is invalid, the registration is aborted,
 * and an error is logged.
 *
 * @param update_func the observer function to be registered for receiving updates
 */
void observe_local(void (*update_func)(void));

/**
 * Shut down the local language handler by releasing resources and closing the resource file.
 *
 * This function deallocates all memory used by the observer list tied to the local handler. It ensures that
 * all dynamically allocated observer nodes are freed to prevent memory leaks. Additionally, it closes the
 * language resource file if it was previously opened.
 *
 * Proper usage of this function ensures a clean shutdown of the local language handler by releasing
 * occupied resources and leaving no lingering operations.
 */
void shutdown_local_handler(void);

#endif//LOCAL_HANDLER_H
