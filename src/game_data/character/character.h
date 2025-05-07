#ifndef CHARACTER_H
#define CHARACTER_H

#include "../../memory/mem_mgmt.h"
#include "stats.h"
#include "../ability/ability.h"

typedef struct ability_node {
    ability_t* ability;
    struct ability_node* next;
} ability_node_t;

typedef struct character {
    unsigned int id;//currently not in use
    unsigned int level;
    char* name;

    resources_t base_resources;   // base = character without gear
    resources_t max_resources;    // max = base + buffs from gear
    resources_t current_resources;// current usable resources, current <= max

    attributes_t base_attributes;   // base = character without gear
    attributes_t max_attributes;    // max = base + buffs from gear
    attributes_t current_attributes;// current = max +- temporary buffs / debuffs

    ability_node_t* abilities; // linked list of abilities
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
character_t* create_base_character(const memory_pool_t* pool, unsigned int id, const char* name);

/**
 * Creates a character with specified ID, name, level, base resources, and base attributes,
 * using the provided memory pool for memory allocation.
 *
 * The character's resources and attributes are initialized according to the given
 * base values, while ensuring no resource or attribute value is below 1. The level is
 * constrained to the range [1, 20]. All max and current resources and attributes are
 * initialized with the specified base values.
 *
 * @param pool A pointer to the memory pool used for allocating memory for the character. Must not be NULL.
 * @param id The unique identifier for the character.
 * @param name A pointer to the name of the character. The name must not be NULL.
 * @param level The initial level of the character. The level is clamped between 1 and 20.
 * @param base_res A set of base resources (health, stamina, mana) to initialize the character with.
 *                 Values below 1 will be adjusted to 1.
 * @param base_attr A set of base attributes (strength, intelligence, agility, endurance, luck)
 *                  to initialize the character with. Values below 1 will be adjusted to 1.
 * @return A pointer to the newly created character structure, or NULL if memory allocation fails
 *         or any mandatory parameter is invalid.
 */
character_t* create_character(const memory_pool_t* pool, unsigned int id, const char* name, int level, resources_t base_res, attributes_t base_attr);

/**
 * Destroys a character object by freeing its allocated memory.
 *
 * @param pool A pointer to the memory pool used to manage memory allocation. Must not be NULL.
 * @param character A pointer to the character object to be destroyed. Must be a valid pointer or NULL.
 */
void destroy_character(const memory_pool_t* pool, character_t* character);

/**
 * Adds resource values (health, stamina, and mana) to a character, ensuring that
 * current resources do not exceed their respective maximum values.
 *
 * @param character A pointer to the character whose resources are to be modified. Must not be NULL.
 * @param health The amount of health to add to the character. Values are clamped so that the total does not exceed max health.
 * @param stamina The amount of stamina to add to the character. Values are clamped so that the total does not exceed max stamina.
 * @param mana The amount of mana to add to the character. Values are clamped so that the total does not exceed max mana.
 */
void add_resources_c(character_t* character, unsigned int health, unsigned int stamina, unsigned int mana);

/**
 * Resets the resources of the specified character to their maximum values.
 *
 * @param character A pointer to the character whose resources are to be reset. Must not be NULL.
 */
void reset_resources_c(character_t* character);

/**
 * Resets the health of the specified character to its maximum value.
 *
 * @param character A pointer to the character whose health needs to be reset. Must not be NULL.
 */
void reset_health_c(character_t* character);

/**
 * Resets the stamina of the specified character to its maximum value.
 *
 * @param character A pointer to the character whose stamina is to be reset. Must not be NULL.
 */
void reset_stamina_c(character_t* character);

/**
 * Resets the mana of the specified character to its maximum value.
 *
 * @param character A pointer to the character whose mana is to be reset. Must not be NULL.
 */
void reset_mana_c(character_t* character);

/**
 * Levels up the specified character, increasing its level and updating its resources
 * and attributes based on predetermined calculations. One primary attribute can be
 * chosen for an additional increase upon leveling up.
 *
 * @param character A pointer to the character to level up. Must not be NULL.
 *                  If the character is already at the maximum level (20), no changes
 *                  will be made, and a warning message will be logged.
 * @param attr_to_increase The identifier of the attribute to increase upon leveling up.
 *                         Acceptable values are: STRENGTH, INTELLIGENCE, AGILITY,
 *                         ENDURANCE, and LUCK. If an invalid attribute identifier
 *                         is provided, an error message will be logged and no
 *                         attribute is increased.
 */
void level_up_c(character_t* character, attr_identifier_t attr_to_increase);

/**
 * Adds a new ability to the character's list of abilities.
 * Allocates memory for a new ability node and appends it to the end
 * of the character's linked list of abilities.
 *
 * @param character A pointer to the character structure to which the ability is added. Must not be NULL.
 * @param ability_id The identifier of the ability to be added, referencing an entry in the ability table.
 */
void add_ability_c(const character_t* character, ability_id_t ability_id);

/**
 * Checks if a character has the specified ability and returns a success flag.
 *
 * @param character A pointer to the character whose abilities are being checked. Must not be NULL.
 * @param ability_id The unique identifier of the ability to be searched for within the character's abilities.
 * @return 1 if the specified ability is found, or 0 if the ability is not present or the parameters are invalid.
 */
int remove_ability_c(const character_t* character, ability_id_t ability_id);

#endif//CHARACTER_H
