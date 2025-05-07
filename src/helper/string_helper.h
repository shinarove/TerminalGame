#ifndef STRING_HELPER_H
#define STRING_HELPER_H


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

#endif//STRING_HELPER_H
