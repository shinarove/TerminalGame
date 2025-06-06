#include "character_output.h"

#include "../../../../termbox2/termbox2.h"
#include "../../../logger/logger.h"
#include "../../local/local_handler.h"
#include "../../string_formats.h"
#include "../../colors.h"

#include <stdio.h>

// NOTE: column width is defined as the space between the columns, which means:
// "|xXxXx|" has a column width of 5.

// for horizontal printing
#define COLUMN_WIDTH_H 16
#define COLUMN1_OFFSET_H 0
#define COLUMN2_OFFSET_H (COLUMN_WIDTH_H)
#define COLUMN3_OFFSET_H (COLUMN2_OFFSET_H + COLUMN_WIDTH_H + 1)
#define COLUMN4_OFFSET_H (COLUMN3_OFFSET_H + COLUMN_WIDTH_H + 1)
#define COLUMN5_OFFSET_H (COLUMN4_OFFSET_H + COLUMN_WIDTH_H + 1)

// for vertical printing (LV means for full string / SV means for short string)
#define COLUMN_WIDTH_V 7
#define COLUMN1_OFFSET_V 0
#define COLUMN2_OFFSET_LV 16
#define COLUMN2_OFFSET_SV 5
#define COLUMN3_OFFSET_LV (COLUMN2_OFFSET_LV + COLUMN_WIDTH_V)
#define COLUMN3_OFFSET_SV (COLUMN2_OFFSET_SV + COLUMN_WIDTH_V)

enum co_str_index {
    LEVEL_STR,
    HEALTH_STR,
    STAMINA_STR,
    MANA_STR,
    HEALTH_SHORT_STR,
    STAMINA_SHORT_STR,
    MANA_SHORT_STR,
    STRENGTH_STR,
    INTELLIGENCE_STR,
    AGILITY_STR,
    CONSTITUTION_STR,
    LUCK_STR,
    STRENGTH_SHORT_STR,
    INTELLIGENCE_SHORT_STR,
    AGILITY_SHORT_STR,
    CONSTITUTION_SHORT_STR,
    LUCK_SHORT_STR,
    MAX_CO_STRINGS
};

char** co_strings = NULL;

void print_attr_h(int x, int y, const attributes_t* attr_base, const attributes_t* attr_bonus);

void print_attr_v(int x, int y, const attributes_t* attr_base, const attributes_t* attr_bonus);

void update_character_output_local(void);

int init_character_output() {
    co_strings = (char**) malloc(sizeof(char*) * MAX_CO_STRINGS);
    RETURN_WHEN_NULL(co_strings, 1, "Character Output", "Failed to allocate memory for character output strings.")

    for (int i = 0; i < MAX_CO_STRINGS; i++) {
        co_strings[i] = NULL;
    }

    update_character_output_local();
    observe_local(update_character_output_local);
    return 0;
}

void print_char_h(const int x, int y, const Character* character, const output_args_c_t args) {
    RETURN_WHEN_NULL(co_strings, , "Character Output", "Module not initialized.")
    RETURN_WHEN_NULL(character, , "Character Output", "In `print_c_res_attr_hori` given player is NULL.")

    const uintattr_t c_white = color_mapping[WHITE].value;
    const uintattr_t c_default = color_mapping[DEFAULT].value;

    // print name and level
    char* char_name = character->id == 0 ? character->name : get_local_string(character->name);
    tb_printf(x, y++, c_white, c_default, NAME_LVL_FORMAT_C,
        char_name, co_strings[LEVEL_STR], character->level);
    if (character->id != 0) free(char_name);

    // prepare offset for the resource strings
    int offset = args.arg_short == 0 ? HEALTH_STR : HEALTH_SHORT_STR;
    // print the resources
    tb_printf(x + COLUMN1_OFFSET_H, y, c_white, c_default, "%s  ", co_strings[offset]);
    tb_printf(x + COLUMN2_OFFSET_H, y, c_white, c_default, "| %s  ", co_strings[offset + 1]);
    tb_printf(x + COLUMN3_OFFSET_H, y, c_white, c_default, "| %s  ", co_strings[offset + 2]);
    y++;
    if (args.arg_res == RES_CURR_MAX) {
        tb_printf(x + COLUMN1_OFFSET_H, y, c_white, c_default, "%d/%d  ",
            character->current_resources.health, character->max_resources.health);
        tb_printf(x + COLUMN2_OFFSET_H, y, c_white, c_default, "| %d/%d  ",
            character->current_resources.stamina, character->max_resources.stamina);
        tb_printf(x + COLUMN3_OFFSET_H, y, c_white, c_default, "| %d/%d  ",
            character->current_resources.mana, character->max_resources.mana);
    } else {
        tb_printf(x + COLUMN1_OFFSET_H, y, c_white, c_default, "%d  ", character->base_resources.health);
        tb_printf(x + COLUMN2_OFFSET_H, y, c_white, c_default, "| %d  ", character->base_resources.stamina);
        tb_printf(x + COLUMN3_OFFSET_H, y, c_white, c_default, "| %d  ", character->base_resources.mana);
    }
    y++;

    // prepare offset for the attribute strings
    offset = args.arg_short == 0 ? STRENGTH_STR : STRENGTH_SHORT_STR;
    const attributes_t attr = args.arg_attr == ATTR_BASE ? character->base_attributes : character->max_attributes;
    // print the attributes
    tb_printf(x + COLUMN1_OFFSET_H, y, c_white, c_default, "%s  ", co_strings[offset]);
    tb_printf(x + COLUMN2_OFFSET_H, y, c_white, c_default, "| %s  ", co_strings[offset + 1]);
    tb_printf(x + COLUMN3_OFFSET_H, y, c_white, c_default, "| %s  ", co_strings[offset + 2]);
    tb_printf(x + COLUMN4_OFFSET_H, y, c_white, c_default, "| %s  ", co_strings[offset + 3]);
    tb_printf(x + COLUMN5_OFFSET_H, y, c_white, c_default, "| %s  ", co_strings[offset + 4]);
    y++;
    tb_printf(x + COLUMN1_OFFSET_H, y, c_white, c_default, "%d  ", attr.strength);
    tb_printf(x + COLUMN2_OFFSET_H, y, c_white, c_default, "| %d  ", attr.intelligence);
    tb_printf(x + COLUMN3_OFFSET_H, y, c_white, c_default, "| %d  ", attr.agility);
    tb_printf(x + COLUMN4_OFFSET_H, y, c_white, c_default, "| %d  ", attr.constitution);
    tb_printf(x + COLUMN5_OFFSET_H, y, c_white, c_default, "| %d  ", attr.luck);
    y++;

    // pint the base & bonus attr info, when needed
    if (args.arg_attr == ATTR_MAX_BONUS) {
        if (character->inventory == NULL) {
            log_msg(WARNING, "Character Output",
                "In `print_c_attr_base_bonus` character's inventory is not initialized");
        } else {
            print_attr_h(x, y,
                &character->base_attributes, &character->inventory->total_attribute_bonus);
        }
    }

    tb_present();
}

void print_char_v(int x, int y, const Character* character, const output_args_c_t args) {
    RETURN_WHEN_NULL(co_strings, , "Character Output", "Module not initialized.")
    RETURN_WHEN_NULL(character, , "Character Output", "In `print_c_res_attr_hori` given player is NULL.")

    const uintattr_t c_white = color_mapping[WHITE].value;
    const uintattr_t c_default = color_mapping[DEFAULT].value;

    // print name and level
    char* char_name = character->id == 0 ? character->name : get_local_string(character->name);
    tb_printf(x++, y++, c_white, c_default, NAME_LVL_FORMAT_C,
        char_name, co_strings[LEVEL_STR], character->level);
    if (character->id != 0) free(char_name);

    // prepare offset for the resource strings
    int str_offset = args.arg_short == 0 ? HEALTH_STR : HEALTH_SHORT_STR;
    // print the resources
    tb_printf(x, y, c_white, c_default, "%s: ", co_strings[str_offset]);
    tb_printf(x, y + 1, c_white, c_default, "%s: ", co_strings[str_offset + 1]);
    tb_printf(x, y + 2, c_white, c_default, "%s: ", co_strings[str_offset + 2]);
    int x_offset = args.arg_short == 0 ? COLUMN2_OFFSET_LV : COLUMN2_OFFSET_SV;
    if (args.arg_res == RES_CURR_MAX) {
        tb_printf(x + x_offset, y, c_white, c_default, "%d/%d ",
            character->current_resources.health, character->max_resources.health);
        tb_printf(x + x_offset, y + 1, c_white, c_default, "%d/%d ",
            character->current_resources.stamina, character->max_resources.stamina);
        tb_printf(x + x_offset, y + 2, c_white, c_default, "%d/%d ",
            character->current_resources.mana, character->max_resources.mana);
    } else {
        tb_printf(x + x_offset, y, c_white, c_default, "%d ", character->base_resources.health);
        tb_printf(x + x_offset, y + 1, c_white, c_default, "%d ", character->base_resources.stamina);
        tb_printf(x + x_offset, y + 2, c_white, c_default, "%d ", character->base_resources.mana);
    }
    y += 4;

    // prepare offset for the attribute strings
    str_offset = args.arg_short == 0 ? STRENGTH_STR : STRENGTH_SHORT_STR;
    const attributes_t attr = args.arg_attr == ATTR_MAX ? character->max_attributes : character->base_attributes;
    // print the attributes
    tb_printf(x, y, c_white, c_default, "%s: ", co_strings[str_offset]);
    tb_printf(x, y + 1, c_white, c_default, "%s: ", co_strings[str_offset + 1]);
    tb_printf(x, y + 2, c_white, c_default, "%s: ", co_strings[str_offset + 2]);
    tb_printf(x, y + 3, c_white, c_default, "%s: ", co_strings[str_offset + 3]);
    tb_printf(x, y + 4, c_white, c_default, "%s: ", co_strings[str_offset + 4]);
    x_offset = args.arg_short == 0 ? COLUMN2_OFFSET_LV : COLUMN2_OFFSET_SV;
    tb_printf(x + x_offset, y, c_white, c_default, "%d ", attr.strength);
    tb_printf(x + x_offset, y + 1, c_white, c_default, "%d ", attr.intelligence);
    tb_printf(x + x_offset, y + 2, c_white, c_default, "%d ", attr.agility);
    tb_printf(x + x_offset, y + 3, c_white, c_default, "%d ", attr.constitution);
    tb_printf(x + x_offset, y + 4, c_white, c_default, "%d ", attr.luck);

    x_offset = args.arg_short == 0 ? COLUMN3_OFFSET_LV : COLUMN3_OFFSET_SV;
    // pint the base & bonus attr info, when needed
    if (args.arg_attr == ATTR_MAX_BONUS) {
        if (character->inventory == NULL) {
            log_msg(WARNING, "Character Output",
                "In `print_c_attr_base_bonus` character's inventory is not initialized");
        } else {
            print_attr_v(x + x_offset, y,
                &character->base_attributes, &character->inventory->total_attribute_bonus);
        }
    }

    tb_present();
}

void shutdown_character_output(void) {
    for (int i = 0; i < MAX_CO_STRINGS; i++) {
        if (co_strings[i] != NULL) {
            free(co_strings[i]);
        }
    }
    free(co_strings);
    co_strings = NULL;
}

void print_attr_h(const int x, const int y, const attributes_t* attr_base, const attributes_t* attr_bonus) {
    if (attr_base == NULL || attr_bonus == NULL) {
        log_msg(WARNING, "Character Output", "In `print_attr_h` given attributes struct is NULL");
        return;
    }

    const uintattr_t c_white = color_mapping[WHITE].value;
    const uintattr_t c_default = color_mapping[DEFAULT].value;
    const uintattr_t c_green = color_mapping[GREEN].value;
    const uintattr_t c_red = color_mapping[RED].value;

    int base_x = x + COLUMN1_OFFSET_H;
    int bonus_x = x + COLUMN1_OFFSET_H + 5;

    // strength
    tb_printf(base_x, y, c_white, c_default, "[%d]", attr_base->strength);
    if (attr_bonus->strength > 0) {
        tb_printf(bonus_x, y, c_green, c_default, "+%d", attr_bonus->strength);
    } else if (attr_bonus->strength < 0) {
        tb_printf(bonus_x, y, c_red, c_default, "%d", attr_bonus->strength);
    } else {
        tb_printf(bonus_x, y, c_white, c_default, "-  ");
    }
    base_x += COLUMN_WIDTH_H;
    bonus_x += COLUMN_WIDTH_H + 2;
    // intelligence
    tb_printf(base_x, y, c_white, c_default, "| [%d]", attr_base->intelligence);
    if (attr_bonus->intelligence > 0) {
        tb_printf(bonus_x, y, c_green, c_default, "+%d ", attr_bonus->intelligence);
    } else if (attr_bonus->intelligence < 0) {
        tb_printf(bonus_x, y, c_red, c_default, "%d ", attr_bonus->intelligence);
    } else {
        tb_printf(bonus_x, y, c_white, c_default, "-  ");
    }
    base_x += COLUMN_WIDTH_H;
    bonus_x += COLUMN_WIDTH_H;
    // agility
    tb_printf(base_x, y, c_white, c_default, "| [%d]", attr_base->agility);
    if (attr_bonus->agility > 0) {
        tb_printf(bonus_x, y, c_green, c_default, "+%d ", attr_bonus->agility);
    } else if (attr_bonus->agility < 0) {
        tb_printf(bonus_x, y, c_red, c_default, "%d ", attr_bonus->agility);
    } else {
        tb_printf(bonus_x, y, c_white, c_default, "-  ");
    }
    base_x += COLUMN_WIDTH_H;
    bonus_x += COLUMN_WIDTH_H;
    // constitution
    tb_printf(base_x, y, c_white, c_default, "| [%d]", attr_base->constitution);
    if (attr_bonus->constitution > 0) {
        tb_printf(bonus_x, y, c_green, c_default, "+%d ", attr_bonus->constitution);
    } else if (attr_bonus->constitution < 0) {
        tb_printf(bonus_x, y, c_red, c_default, "%d ", attr_bonus->constitution);
    } else {
        tb_printf(bonus_x, y, c_white, c_default, "-  ");
    }
    base_x += COLUMN_WIDTH_H;
    bonus_x += COLUMN_WIDTH_H;
    // luck
    tb_printf(base_x, y, c_white, c_default, "| [%d]", attr_base->luck);
    if (attr_bonus->luck > 0) {
        tb_printf(bonus_x, y, c_green, c_default, "+%d ", attr_bonus->luck);
    } else if (attr_bonus->luck < 0) {
        tb_printf(bonus_x, y, c_red, c_default, "%d ", attr_bonus->luck);
    } else {
        tb_printf(bonus_x, y, c_white, c_default, "-  ");
    }
}

void print_attr_v(const int x, int y, const attributes_t* attr_base, const attributes_t* attr_bonus) {
    if (attr_base == NULL || attr_bonus == NULL) {
        log_msg(WARNING, "Character Output", "In `print_attr_v` given attributes struct is NULL");
        return;
    }

    const uintattr_t c_white = color_mapping[WHITE].value;
    const uintattr_t c_default = color_mapping[DEFAULT].value;
    const uintattr_t c_green = color_mapping[GREEN].value;
    const uintattr_t c_red = color_mapping[RED].value;

    const int bonus_x = x + 5;
    // strength
    tb_printf(x, y, c_white, c_default, "[%d]", attr_base->strength);
    if (attr_bonus->strength > 0) {
        tb_printf(bonus_x, y++, c_green, c_default, "+%d", attr_bonus->strength);
    } else if (attr_bonus->strength < 0) {
        tb_printf(bonus_x, y++, c_red, c_default, "%d", attr_bonus->strength);
    } else {
        tb_printf(bonus_x, y++, c_white, c_default, "-  ");
    }
    // intelligence
    tb_printf(x, y, c_white, c_default, "[%d]", attr_base->intelligence);
    if (attr_bonus->intelligence > 0) {
        tb_printf(bonus_x, y++, c_green, c_default, "+%d ", attr_bonus->intelligence);
    } else if (attr_bonus->intelligence < 0) {
        tb_printf(bonus_x, y++, c_red, c_default, "%d ", attr_bonus->intelligence);
    } else {
        tb_printf(bonus_x, y++, c_white, c_default, "-  ");
    }
    // agility
    tb_printf(x, y, c_white, c_default, "[%d]", attr_base->agility);
    if (attr_bonus->agility > 0) {
        tb_printf(bonus_x, y++, c_green, c_default, "+%d ", attr_bonus->agility);
    } else if (attr_bonus->agility < 0) {
        tb_printf(bonus_x, y++, c_red, c_default, "%d ", attr_bonus->agility);
    } else {
        tb_printf(bonus_x, y++, c_white, c_default, "-  ");
    }
    // constitution
    tb_printf(x, y, c_white, c_default, "[%d]", attr_base->constitution);
    if (attr_bonus->constitution > 0) {
        tb_printf(bonus_x, y++, c_green, c_default, "+%d ", attr_bonus->constitution);
    } else if (attr_bonus->constitution < 0) {
        tb_printf(bonus_x, y++, c_red, c_default, "%d ", attr_bonus->constitution);
    } else {
        tb_printf(bonus_x, y++, c_white, c_default, "-  ");
    }
    // luck
    tb_printf(x, y, c_white, c_default, "[%d]", attr_base->luck);
    if (attr_bonus->luck > 0) {
        tb_printf(bonus_x, y, c_green, c_default, "+%d ", attr_bonus->luck);
    } else if (attr_bonus->luck < 0) {
        tb_printf(bonus_x, y, c_red, c_default, "%d ", attr_bonus->luck);
    } else {
        tb_printf(bonus_x, y, c_white, c_default, "-  ");
    }
}


void update_character_output_local(void) {
    for (int i = 0; i < MAX_CO_STRINGS; i++) {
        if (co_strings[i] != NULL) {
            free(co_strings[i]);
        }
    }
    co_strings[LEVEL_STR] = get_local_string("LEVEL");

    co_strings[HEALTH_STR] = get_local_string("HEALTH");
    co_strings[STAMINA_STR] = get_local_string("STAMINA");
    co_strings[MANA_STR] = get_local_string("MANA");
    co_strings[HEALTH_SHORT_STR] = get_local_string("HEALTH.SHORT");
    co_strings[STAMINA_SHORT_STR] = get_local_string("STAMINA.SHORT");
    co_strings[MANA_SHORT_STR] = get_local_string("MANA.SHORT");

    co_strings[STRENGTH_STR] = get_local_string("STRENGTH");
    co_strings[INTELLIGENCE_STR] = get_local_string("INTELLIGENCE");
    co_strings[AGILITY_STR] = get_local_string("AGILITY");
    co_strings[CONSTITUTION_STR] = get_local_string("CONSTITUTION");
    co_strings[LUCK_STR] = get_local_string("LUCK");
    co_strings[STRENGTH_SHORT_STR] = get_local_string("STRENGTH.SHORT");
    co_strings[INTELLIGENCE_SHORT_STR] = get_local_string("INTELLIGENCE.SHORT");
    co_strings[AGILITY_SHORT_STR] = get_local_string("AGILITY.SHORT");
    co_strings[CONSTITUTION_SHORT_STR] = get_local_string("CONSTITUTION.SHORT");
    co_strings[LUCK_SHORT_STR] = get_local_string("LUCK.SHORT");
}
