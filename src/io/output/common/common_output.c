#include "common_output.h"

#include "../../../logger/logger.h"
#include "../../../../termbox2/termbox2.h"

void clear_screen() {
    tb_clear();
}

void print_text(int x, int y, const color_t fg, const color_t bg, const char* text) {
    RETURN_WHEN_NULL(text, , "Common Output", "print_text: Text is NULL");

    if (x < 0 || y < 0) {
        log_msg(WARNING, "Common Output", "Invalid anchor position: (%d, %d), set to (0, 0)", x, y);
        x = 0;
        y = 0;
    }

    tb_printf(x, y, color_mapping[fg].value, color_mapping[bg].value, "%s", text);
    tb_present();
}

void print_menu(int x, int y, const menu_t* menu) {
    RETURN_WHEN_NULL(menu, , "Common Output", "print_menu: Menu is NULL");

    if (x < 0 || y < 0) {
        log_msg(WARNING, "Common Output", "Invalid anchor position: (%d, %d), set to (0, 0)", x, y);
        x = 0;
        y = 0;
    }

    //print title
    tb_printf(x, y++, TB_WHITE, TB_DEFAULT, "%s", menu->title);

    for (int i = 0; i < menu->option_count; i++) {
        if (i == menu->selected_index) {
            tb_printf(x, y++, TB_BLACK, TB_WHITE, "> %s", menu->options[i]);
        } else {
            tb_printf(x, y++, TB_WHITE, TB_DEFAULT, "  %s", menu->options[i]);
        }
    }

    tb_printf(x, y + 2, TB_WHITE, TB_DEFAULT, "%s", menu->tailing_text);
    tb_present();
}
