#ifndef CHARACTER_SAVE_HANDLER_H
#define CHARACTER_SAVE_HANDLER_H

#include "character.h"

#include <stdio.h>

/**
 * Writes character data to a file.
 *
 * This function serializes and writes the provided character's data
 * into the specified file. It includes the character's ID, experience,
 * level, attributes, resources, name, abilities, and other relevant
 * information. Abilities are written as their respective IDs.
 *
 * @param file A pointer to the file where the character data will be written.
 *             Must be a valid, open file pointer.
 * @param character A pointer to the `character_t` structure
 *                  representing the character to write. Must not be NULL.
 * @return 0 if the operation succeeds, or 1 if either `file` or `character`
 *         is NULL or if an error occurs during writing.
 */
int write_character_data(FILE* file, const character_t* character);

int read_character_data(FILE* file, character_t* character);

long calculate_checksum_c(const character_t* character);

#endif //CHARACTER_SAVE_HANDLER_H
