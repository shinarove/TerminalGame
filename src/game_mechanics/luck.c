#include "luck.h"

#include "../logger/logger.h"
#include "dice/dice.h"

int roll_luck_dice(const character_t* character) {
    RETURN_WHEN_NULL(character, 0, "Luck", "In `roll_luck_dice` given character is NULL")
    int res = 0;
    dice_t used_dice = D3;

    const int c_luck = character->current_attributes.luck;
    if (c_luck > 24) {
        used_dice = D20;
    } else if (c_luck > 20) {
        used_dice = D12;
    } else if (c_luck > 16) {
        used_dice = D10;
    } else if (c_luck > 12) {
        used_dice = D8;
    } else if (c_luck > 8) {
        used_dice = D6;
    } else if (c_luck > 4) {
        used_dice = D4;
    } else if (c_luck >= 1) {
        used_dice = D3;
    } else {
        log_msg(WARNING, "Luck", "In `roll_luck_dice` character has invalid luck value %d", c_luck);
        return 0;
    }

    res = roll_dice(used_dice) - 1; // that way a 0 res is possible
    return res;
}
