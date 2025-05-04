#ifndef CHARACTER_PARSER_H
#define CHARACTER_PARSER_H

#include "character.h"

/**
 * Converts a given character object to a CSV-formatted string.
 * The generated string contains the character's ID, name, resources (base, max, current),
 * and attributes (base, max, current) separated by semicolons.
 *
 * @param character A pointer to the character_t struct representing the character to be converted.
 * @return A pointer to a dynamically allocated null-terminated string containing
 *         the character's data in CSV format. The caller is responsible for freeing this memory.
 *         Returns NULL if memory allocation fails.
 */
char* character_to_csv(const character_t* character);

#endif //CHARACTER_PARSER_H
