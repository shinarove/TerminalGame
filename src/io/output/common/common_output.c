#include "common_output.h"

#include "../../../logger/logger.h"
#include "../../../../termbox2/termbox2.h"

void print_text(const int x, const int y, const color_t fg, const color_t bg, const char* text) {
    RETURN_WHEN_NULL(text, , "Common Output", "print_text: Text is NULL");

    tb_printf(x, y, color_mapping[fg].value, color_mapping[bg].value, "%s", text);
}
