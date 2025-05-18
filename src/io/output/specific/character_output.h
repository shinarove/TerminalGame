#ifndef CHARACTER_OUTPUT_H
#define CHARACTER_OUTPUT_H

#include "../../../game_data/character/character.h"

/**
 * A structure to hold the arguments for the character output subsystem.
 */
typedef struct output_args_c {
    bool update; // if true the character information is updated in the cache
    bool show_res_curr_max; // if true the max resources are shown
    bool show_attr_max; // if true the max attributes are shown, otherwise the base attributes are shown
} output_args_c_t;

/**
 * Initializes the character output subsystem.
 *
 * @return 0 on successful initialization of the character output subsystem.
 *         Returns 1 if memory allocation or string cache creation fails.
 */
int init_character_output(void);

/**
 * Displays detailed information about a character at a specified position on the screen.
 * This includes the character's name, level, resources, and attributes.
 * If the `update` flag is set, the character's information is refreshed in the cache.
 *
 * @param x The x-coordinate on the screen where the information is printed.
 * @param y The y-coordinate on the screen where the information starts printing. It increments with each line.
 * @param character Pointer to the character whose information needs to be displayed.
 * @param args A structure containing diverse flags.
 */
void print_info_c(int x, int y, const character_t* character, output_args_c_t args);

/**
 * Shuts down the character output subsystem, releasing all associated resources.
 *
 * Frees memory allocated for character output strings and destroys the
 * character cache to ensure proper cleanup of the subsystem.
 */
void shutdown_character_output(void);

#endif//CHARACTER_OUTPUT_H
