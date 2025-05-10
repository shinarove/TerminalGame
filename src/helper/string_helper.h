#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <stdlib.h>

/**
 * Analyzes a string and converts it to an integer if the string represents
 * a valid integer within the range of an int.
 *
 * @param str A null-terminated string to be analyzed.
 *            Must not be NULL or empty.
 * @param out A pointer to an integer where the parsed value will be stored
 *            if the analysis is successful. Must not be NULL.
 *
 * @return 0 if the string was successfully analyzed and converted to an integer.
 *         -1 if the input string is NULL, empty, not a valid integer,
 *         out of the integer range, or otherwise invalid.
 */
int parse_int(const char* str, int* out);

/**
 * Parses a string and converts it to a float if the string represents
 * a valid floating-point number within the range of a float.
 *
 * @param str A null-terminated string to be parsed. Must not be NULL or empty.
 * @param out A pointer to a float where the parsed value will be stored
 *            if parsing is successful. Must not be NULL.
 *
 * @return 0 on successful parsing and conversion of the string to a float.
 *         -1 if the input string is NULL, empty, not a valid float,
 *         out of the range of a float, or represents a NaN or infinity.
 */
int parse_float(const char* str, float* out);


/**
 * Formats the current date and time into ISO 8601 format and writes
 * it into the provided buffer.
 *
 * @param buffer A pointer to a character array where the formatted date
 *               and time will be stored. Must not be NULL.
 * @param buffer_size The size of the buffer in bytes. Must be large
 *                    enough to store the formatted date and time,
 *                    including the null terminator.
 *
 * @return The number of characters written (excluding the null
 *         terminator) if successful. Returns 0 if the buffer is too
 *         small.
 */
size_t str_iso_time(char* buffer, size_t buffer_size);

#endif//STRING_HELPER_H
