#ifndef COLORS_H
#define COLORS_H

#include "../../termbox2/termbox2.h"

typedef enum {
    DEFAULT,
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    MAX_COLORS
} color_t;

static struct {
    color_t key;
    uintattr_t value;
} color_mapping[] = {
        {DEFAULT, TB_DEFAULT},
        {BLACK, TB_BLACK},
        {RED, TB_RED},
        {GREEN, TB_GREEN},
        {YELLOW, TB_YELLOW},
        {BLUE, TB_BLUE},
        {MAGENTA, TB_MAGENTA},
        {CYAN, TB_CYAN},
        {WHITE, TB_WHITE}};

#endif//COLORS_H
