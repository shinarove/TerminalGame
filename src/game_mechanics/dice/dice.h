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
    NO_DICE = 0,
    D3 = 3,
    D4 = 4,
    D6 = 6,
    D8 = 8,
    D10 = 10,
    D12 = 12,
    D20 = 20
} dice_t;

#endif//DICE_H
