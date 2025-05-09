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

/**
 * Extracts the menu arguments. If the passed argument is null, a default menu
 * configuration with specific colors and inactive state is returned.
 *
 * @param args Pointer to a `menu_arg_t` structure containing the menu arguments.
 *             If null, default menu arguments will be used.
 * @return A `menu_arg_t` structure representing the extracted menu arguments or
 *         default values if the input is null.
 */
menu_arg_t extract_menu_args(const menu_arg_t* args);

void clear_screen() {
    tb_clear();
}

void print_text(int x, int y, const color_t fg, const color_t bg, const char* text) {
    RETURN_WHEN_NULL(text, , "Common Output", "In `print_text` text is NULL");
    check_xy(&x, &y);

    tb_printf(x, y, color_mapping[fg].value, color_mapping[bg].value, "%s", text);
    tb_present();
}

void print_simple_menu(int x, int y, const menu_t* menu) {
    RETURN_WHEN_NULL(menu, , "Common Output", "In `print_simple_menu` menu is NULL");
    check_xy(&x, &y);

    const menu_arg_t args = extract_menu_args(menu->args);
    const uintattr_t uns_fg = color_mapping[args.unselected_fg].value;
    const uintattr_t uns_bg = color_mapping[args.unselected_bg].value;
    const uintattr_t sel_fg = color_mapping[args.selected_fg].value;
    const uintattr_t sel_bg = color_mapping[args.selected_bg].value;

    //print title
    tb_printf(x, y++, uns_fg, uns_bg, "%s", menu->title);

    for (int i = 0; i < menu->option_count; i++) {
        if (i == menu->selected_index && args.active) {
            tb_printf(x, y++, sel_fg, sel_bg, "> %s", menu->options[i]);
        } else {
            tb_printf(x, y++, uns_fg, uns_bg, "  %s", menu->options[i]);
        }
    }

    tb_printf(x, y + 2, uns_fg, uns_bg, "%s", menu->tailing_text);
    tb_present();
}

void print_spinner_menu(int x, int y, const spinner_menu_t* spinner_menu) {
    RETURN_WHEN_NULL(spinner_menu, , "Common Output", "In `print_spinner_menu` menu is NULL")
    RETURN_WHEN_NULL(spinner_menu->menu, , "Common Output", "In `print_spinner_menu` additional_info is NULL")
    RETURN_WHEN_TRUE(spinner_menu->max_option_length < 0, , "Common Output",
                     "In `print_spinner_menu` max_option_length is negative")
    check_xy(&x, &y);

    const int spinner_x_pos = x + args->max_option_length + 1;
    //print title
    tb_printf(x, y++, TB_WHITE, TB_DEFAULT, "%s", spinner_menu->title);

    for (int i = 0; i < spinner_menu->option_count; i++) {
        tb_printf(x, y, TB_WHITE, TB_DEFAULT, "%s", spinner_menu->options[i]);
        if (i == spinner_menu->selected_index / 2 && spinner_menu->selected_index % 2 == 0) {
            // the left symbol is marked
            tb_printf(spinner_x_pos, y, TB_BLACK, TB_WHITE, "%c", args->left_symbol);
            tb_printf(spinner_x_pos + 2, y, TB_WHITE, TB_DEFAULT, "%c", args->right_symbol);
        } else if (i == spinner_menu->selected_index / 2 && spinner_menu->selected_index % 2 == 1) {
            // the right symbol is marked
            tb_printf(spinner_x_pos, y, TB_WHITE, TB_DEFAULT, "%c", args->left_symbol);
            tb_printf(spinner_x_pos + 2, y, TB_BLACK, TB_WHITE, "%c", args->right_symbol);
        } else {
            tb_printf(spinner_x_pos, y, TB_WHITE, TB_DEFAULT, "%c", args->left_symbol);
            tb_printf(spinner_x_pos + 2, y, TB_WHITE, TB_DEFAULT, "%c", args->right_symbol);
        }
        y += 1;
    }

    tb_printf(x, y + 2, TB_WHITE, TB_DEFAULT, "%s", spinner_menu->tailing_text);
    tb_present();
}

void check_xy(int* x, int* y) {
    if (*x < 0 || *y < 0) {
        log_msg(WARNING, "Common Output", "Invalid anchor position: (%d, %d), set to (0, 0)", *x, *y);
        *x = 0;
        *y = 0;
    }
}

menu_arg_t extract_menu_args(const menu_arg_t* args) {
    if (args == NULL) {
        return {1, BLACK, WHITE, WHITE, DEFAULT};
    }
    return *args;
}
