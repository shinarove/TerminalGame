#include "character_parser.h"

#include "../../logger/logger.h"

#include <stdio.h>

/**
 * Writes the resource values into a CSV format string, starting from the specified offset.
 * Each resource value (health, stamina, mana) is appended to the CSV line followed by a delimiter.
 *
 * @param csv_line A pointer to the character array where the CSV data will be written.
 * @param csv_offset A pointer to an integer representing the current offset in the csv_line array.
 *                   The function updates this offset as it appends data.
 * @param resources A resources_t structure containing the health, stamina, and mana values to be written into the CSV line.
 */
void put_resources_in_csv(char* csv_line, const int* csv_offset, resources_t resources);

/**
 * Appends the attribute values into a CSV format string, starting from the specified offset.
 * Each attribute value (strength, intelligence, agility, endurance, luck) is written into the CSV line,
 * separated by a delimiter.
 *
 * @param csv_line A pointer to the character array where the CSV data will be appended.
 * @param csv_offset A pointer to an integer representing the current offset in the csv_line array.
 *                   The function updates this offset as it appends the attribute values.
 * @param attributes An attributes_t structure containing the strength, intelligence, agility, endurance,
 *                   and luck values to be written into the CSV line.
 */
void put_attributes_in_csv(char* csv_line, const int* csv_offset, attributes_t attributes);

/**
 * Appends an integer value to a CSV format string at the specified offset.
 * Converts the integer value into a string representation and adds it to the
 * CSV string, followed by a delimiter.
 *
 * @param csv_line A pointer to the character array where the CSV data will be written.
 * @param csv_offset A pointer to an integer representing the current offset in the csv_line array.
 *                   The function updates this offset as it appends data.
 * @param value The integer value to be appended to the CSV line.
 */
void put_int_in_csv(char* csv_line, const int* csv_offset, int value);

char* character_to_csv(const character_t* character) {
    char* csv_line = (char*) malloc(256);
    RETURN_WHEN_NULL(csv_line, NULL, "Character Parser", "Failed to allocate memory for CSV line");

    char int_str[16];
    snprintf(int_str, sizeof(int_str), "%d", character->id);

    int csv_offset = 0;
    int offset = 0;
    while (int_str[offset] != '\0') {
        csv_line[csv_offset++] = int_str[offset++];
    }
    csv_line[csv_offset++] = ';';

    offset = 0;
    while (character->name[offset] != '\0' && offset < sizeof(character->name)) {
        csv_line[csv_offset++] = character->name[offset++];
    }
    csv_line[csv_offset++] = ';';

    put_resources_in_csv(csv_line, &csv_offset, character->base_resources);
    put_resources_in_csv(csv_line, &csv_offset, character->max_resources);
    put_resources_in_csv(csv_line, &csv_offset, character->current_resources);

    put_attributes_in_csv(csv_line, &csv_offset, character->base_attributes);
    put_attributes_in_csv(csv_line, &csv_offset, character->max_attributes);
    put_attributes_in_csv(csv_line, &csv_offset, character->current_attributes);

    csv_line[csv_offset] = '\0';// Null-terminate the string

    return csv_line;
}

void put_resources_in_csv(char* csv_line, const int* csv_offset, const resources_t resources) {
    put_int_in_csv(csv_line, csv_offset, resources.health);
    put_int_in_csv(csv_line, csv_offset, resources.stamina);
    put_int_in_csv(csv_line, csv_offset, resources.mana);
}

void put_attributes_in_csv(char* csv_line, const int* csv_offset, const attributes_t attributes) {
    put_int_in_csv(csv_line, csv_offset, attributes.strength);
    put_int_in_csv(csv_line, csv_offset, attributes.intelligence);
    put_int_in_csv(csv_line, csv_offset, attributes.agility);
    put_int_in_csv(csv_line, csv_offset, attributes.endurance);
    put_int_in_csv(csv_line, csv_offset, attributes.luck);
}

void put_int_in_csv(char* csv_line, const int* csv_offset, const int value) {
    char int_str[16];
    snprintf(int_str, sizeof(int_str), "%d", value);
    int offset = 0;
    while (int_str[offset] != '\0') {
        csv_line[*csv_offset++] = int_str[offset++];
    }
    csv_line[*csv_offset++] = ';';
}
