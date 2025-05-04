#ifndef CHARACTER_H
#define CHARACTER_H

#include "../../memory/mem_mgmt.h"

typedef struct resources {
    unsigned short health;
    unsigned short stamina;
    unsigned short mana;
} resources_t;

typedef struct attributes {
    unsigned short strength;
    unsigned short intelligence;
    unsigned short agility;
    unsigned short endurance;
    unsigned short luck;
} attributes_t;

typedef struct character {
    unsigned int id;
    char name[64];

    resources_t base_resources; // base = character without gear
    resources_t max_resources; // max = base + buffs from gear
    resources_t current_resources; // current usable resources, current <= max

    attributes_t base_attributes; // base = character without gear
    attributes_t max_attributes; // max = base + buffs from gear
    attributes_t current_attributes; // current = max +- temporary buffs / debuffs
} character_t;

/**
 * Creates a base character with specified ID and name, using the provided memory pool.
 * The character's resources and attributes are initialized to default values.
 *
 * @param pool A pointer to the memory pool used for allocating memory for the character. Must not be NULL.
 * @param id The unique identifier for the character.
 * @param name The name of the character. Must not be NULL.
 * @return A pointer to the newly created character structure, or NULL if memory allocation fails or parameters are invalid.
 */
character_t* create_base_character(const memory_pool_t* pool, int id, const char* name);

/**
 * Creates a character with the specified ID, name, base resources, and base attributes
 * using the given memory pool. The character's resources and attributes (base, max, and current)
 * are initialized accordingly.
 *
 * @param pool A pointer to the memory pool used for allocating memory for the character. Must not be NULL.
 * @param id The unique identifier for the character.
 * @param name The name of the character. Must not be NULL.
 * @param base_res The initial base resources of the character (e.g., health, stamina, mana).
 * @param base_attr The initial base attributes of the character (e.g., strength, intelligence, agility, etc.).
 * @return A pointer to the newly created character structure, or NULL if memory allocation fails or parameters are invalid.
 */
character_t* create_character(const memory_pool_t* pool, int id, const char* name, const resources_t base_res, const attributes_t base_attr);

/**
 * Destroys a character object by freeing its allocated memory.
 *
 * @param pool A pointer to the memory pool used to manage memory allocation. Must not be NULL.
 * @param character A pointer to the character object to be destroyed. Must be a valid pointer or NULL.
 */
void destroy_character(const memory_pool_t* pool, character_t* character);

#endif //CHARACTER_H
