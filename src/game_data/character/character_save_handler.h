#ifndef CHARACTER_SAVE_HANDLER_H
#define CHARACTER_SAVE_HANDLER_H

#include "character.h"

#include <stdio.h>

/**
 * Writes character data to a file. Abilities are written as their respective IDs.
 *
 * @param file A pointer to the file where the character data will be written.
 *             Must be a valid, open file pointer.
 * @param character A pointer to the `character_t` structure
 *                  representing the character to write. Must not be NULL.
 * @return 0 if the operation succeeds, or 1 if either `file` or `character`
 *         is NULL or if an error occurs during writing.
 */
int write_character_data(FILE* file, const Character* character);

/**
 * Reads character data from a file. Abilities are expected to be stored by their IDs.
 *
 * @param file A pointer to the file from which the character data
 *             will be read. Must be a valid, open file pointer.
 * @param character A pointer to the `character_t` structure where
 *                  the read data will be stored. Must not be NULL.
 * @return 0 if the operation succeeds, or 1 if `file` or `character`
 *         is NULL, if there is insufficient memory, or if an error
 *         occurs during reading.
 */
int read_character_data(FILE* file, Character* character);

/**
 * Calculates a checksum for the provided character.
 *
 * @param character A pointer to the `character_t` structure
 *                  representing the character whose checksum
 *                  will be calculated. Must not be NULL.
 * @return A long integer representing the calculated checksum
 *         of the character's data, or 0 if the `character`
 *         pointer is NULL.
 */
long calculate_checksum_c(const Character* character);

#endif//CHARACTER_SAVE_HANDLER_H
