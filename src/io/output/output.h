#ifndef OUTPUT_H
#define OUTPUT_H

/**
 * Initializes the output subsystem.
 *
 * @return Returns 0 on successful initialization, or a non-zero value
 *         if an error occurs during the initialization process.
 */
int init_output(void);

/**
 * Shuts down the output subsystem by delegating the shutdown process
 * to the character output module.
 */
void shutdown_output(void);

#endif //OUTPUT_H
