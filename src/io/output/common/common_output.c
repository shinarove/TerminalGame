#include "common_output.h"

#include "../../../../termbox2/termbox2.h"
#include "../../../logger/logger.h"

/**
 * Ensures the provided coordinates are valid. If `x` or `y` are less than 0,
 * they are reset to 0 and a warning is logged.
 *
 * @param x Pointer to the x-coordinate to be checked and adjusted if necessary.
 * @param y Pointer to the y-coordinate to be checked and adjusted if necessary.
 */
void check_xy(int* x, int* y);

void clear_screen() {
    tb_clear();
}

void clear_line(const int y, const int x_start, const int x_end) {
    for (int i = x_start; i < x_end; i++) {
        tb_set_cell(i, y, ' ', color_mapping[DEFAULT].value, color_mapping[DEFAULT].value);
    }
    tb_present();
}

void print_text(int x, int y, const color_t fg, const color_t bg, const char* text) {
    check_xy(&x, &y);

    tb_printf(x, y, color_mapping[fg].value, color_mapping[bg].value, "%s", text);
    tb_present();
}

void print_text_f(int x, int y, const color_t fg, const color_t bg, const char* format, ...) {
    check_xy(&x, &y);

    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    tb_printf(x, y, color_mapping[fg].value, color_mapping[bg].value, "%s", buffer);
    va_end(args);

    tb_present();
}

void print_simple_menu(int x, int y, const Menu* simple_menu) {
    RETURN_WHEN_NULL(simple_menu, , "Common Output", "In `print_simple_menu` menu is NULL")
    RETURN_WHEN_TRUE(simple_menu->args.max_option_length < 0, , "Common Output",
                     "In `print_simple_menu` max_option_length is negative")
    check_xy(&x, &y);

    const uintattr_t uns_fg = color_mapping[simple_menu->args.unselected_fg].value;
    const uintattr_t uns_bg = color_mapping[simple_menu->args.unselected_bg].value;
    const uintattr_t sel_fg = color_mapping[simple_menu->args.selected_fg].value;
    const uintattr_t sel_bg = color_mapping[simple_menu->args.selected_bg].value;

    //print title
    tb_printf(x, y++, uns_fg, uns_bg, "%s", simple_menu->title);

    for (int i = 0; i < simple_menu->option_count; i++) {
        // if (menu->options[i] == NULL) continue;

        if (i == simple_menu->selected_index &&
            simple_menu->args.mode != INACTIVE_WOUT_SEL) {
            tb_printf(x, y++, sel_fg, sel_bg, "> %s", simple_menu->options[i]);
        } else {
            tb_printf(x, y++, uns_fg, uns_bg, "  %s", simple_menu->options[i]);
        }
    }

    tb_printf(x, y + 2, uns_fg, uns_bg, "%s", simple_menu->tailing_text);
    tb_present();
}

void print_spinner_menu(int x, int y, const Menu* spinner_menu) {
    RETURN_WHEN_NULL(spinner_menu, , "Common Output", "In `print_spinner_menu` menu is NULL")
    RETURN_WHEN_TRUE(spinner_menu->args.max_option_length < 0, , "Common Output",
                     "In `print_spinner_menu` max_option_length is negative")
    check_xy(&x, &y);

    const menu_mode_t mode = spinner_menu->args.mode;
    const uintattr_t uns_fg = color_mapping[spinner_menu->args.unselected_fg].value;
    const uintattr_t uns_bg = color_mapping[spinner_menu->args.unselected_bg].value;
    const uintattr_t sel_fg = color_mapping[spinner_menu->args.selected_fg].value;
    const uintattr_t sel_bg = color_mapping[spinner_menu->args.selected_bg].value;

    const int spinner_x_pos = x + spinner_menu->args.max_option_length + 1;
    //print title
    tb_printf(x, y++, uns_fg, uns_bg, "%s", spinner_menu->title);

    for (int i = 0; i < spinner_menu->option_count; i++) {
        tb_printf(x, y, uns_fg, uns_bg, "%s", spinner_menu->options[i]);
        if (i == spinner_menu->selected_index / 2 &&
            spinner_menu->selected_index % 2 == 0 &&
            mode != INACTIVE_WOUT_SEL) {
            // the left symbol is marked
            tb_printf(spinner_x_pos, y, sel_fg, sel_bg, "<");
            tb_printf(spinner_x_pos + 2, y, uns_fg, uns_bg, ">");
        } else if (i == spinner_menu->selected_index / 2 &&
            spinner_menu->selected_index % 2 == 1 &&
            mode != INACTIVE_WOUT_SEL) {
            // the right symbol is marked
            tb_printf(spinner_x_pos, y, uns_fg, uns_bg, "<");
            tb_printf(spinner_x_pos + 2, y, sel_fg, sel_bg, ">");
        } else {
            tb_printf(spinner_x_pos, y, uns_fg, uns_bg, "<");
            tb_printf(spinner_x_pos + 2, y, uns_fg, uns_bg, ">");
        }
        y += 1;
    }

    tb_printf(x, y + 2, uns_fg, uns_bg, "%s", spinner_menu->tailing_text);
    tb_present();
}

void check_xy(int* x, int* y) {
    if (*x < 0 || *y < 0) {
        log_msg(WARNING, "Common Output", "Invalid anchor position: (%d, %d), set to (0, 0)", *x, *y);
        *x = 0;
        *y = 0;
    }
}
