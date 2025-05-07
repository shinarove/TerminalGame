#include "dice.h"

#include <stdlib.h>

unsigned int roll_dice(const dice_t dice) {
    return rand() % dice + 1;
}

int check_dice(const int dice) {
    switch (dice) {
        case D3:
        case D4:
        case D6:
        case D8:
        case D10:
        case D12:
        case D20:
            return 0;
        default:
            return 1;
    }
}
