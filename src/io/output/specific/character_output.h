#ifndef CHARACTER_OUTPUT_H
#define CHARACTER_OUTPUT_H

#include "../../../game_data/character/character.h"

#define RES_BASE 0
#define RES_CURR_MAX 1

#define ATTR_BASE 0
#define ATTR_MAX 1
#define ATTR_MAX_BONUS 2

/**
 * A structure to hold the arguments for the character output subsystem.
 */
typedef struct output_args_c {
    // if 1 the resources and attributes are shown in short form (e.g. STR, AGI)
    unsigned int arg_short : 1;
    // if 1 the current and max resources are shown, otherwise only the base resources are shown
    unsigned int arg_res : 1;
    // if 0 = base attributes, 1 = max attributes, 2 = max attributes + bonus
    unsigned int arg_attr : 2;
} output_args_c_t;

/**
 * Initializes the character output subsystem.
 *
 * @return 0 on successful initialization of the character output subsystem.
 *         Returns 1 if memory allocation or string cache creation fails.
 */
int init_character_output(void);

/**
 * Prints the character's information in a horizontal layout at the specified position.
 *
 * @param x The x-coordinate of the starting position for the output.
 * @param y The y-coordinate of the starting position for the output.
 * @param character A pointer to the character_t structure containing the character's data.
 *                  Must not be NULL.
 * @param args A structure of type output_args_c_t that specifies the formatting options
 *             for the output, such as whether to use short labels or display additional details.
 *
 * @note This function displays the character's resources (e.g., health, stamina, mana) and attributes
 * (e.g., strength, intelligence, agility) in a horizontal format. The output is formatted
 * based on the provided arguments, such as showing short labels or including bonus attributes.
 */
void print_char_h(int x, int y, character_t* character, output_args_c_t args);

void print_char_v(int x, int y, character_t* character, output_args_c_t args);

/**
 * Shuts down the character output subsystem, releasing all associated resources.
 *
 * Frees memory allocated for character output strings and destroys the
 * character cache to ensure proper cleanup of the subsystem.
 */
void shutdown_character_output(void);

#endif//CHARACTER_OUTPUT_H
