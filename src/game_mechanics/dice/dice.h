#ifndef DICE_H
#define DICE_H

#define D3_STR "d3"
#define D4_STR "d4"
#define D6_STR "d6"
#define D8_STR "d8"
#define D10_STR "d10"
#define D12_STR "d12"
#define D20_STR "d20"

typedef enum {
    D3 = 3,
    D4 = 4,
    D6 = 6,
    D8 = 8,
    D10 = 10,
    D12 = 12,
    D20 = 20
} dice_t;

/**
 * A simple function to roll a die.
 *
 * @param dice The used dice for rolling
 * @return The rolled value
 */
int roll_dice(dice_t dice);

/**
 * Verifies if the provided dice value is valid.
 *
 * @param dice The dice value to check
 * @return Returns 0 if the dice is valid, otherwise returns 1
 */
int check_dice(int dice);

#endif//DICE_H
