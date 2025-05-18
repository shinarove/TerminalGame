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

void clear_line(const int y, const int x_start, const int x_end) {
    for (int i = x_start; i < x_end; i++) {
        tb_set_cell(i, y, ' ', color_mapping[DEFAULT].value, color_mapping[DEFAULT].value);
    }
    tb_present();
}

void print_text(int x, int y, const color_t fg, const color_t bg, const char* text) {
    RETURN_WHEN_NULL(text, , "Common Output",
                     "In `print_text` text is NULL with the following config: x: %d, y: %d, fg: %d, bg: %d", x, y, fg, bg);
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
        // if (menu->options[i] == NULL) continue;

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

    const menu_arg_t args = extract_menu_args(spinner_menu->menu->args);
    const uintattr_t uns_fg = color_mapping[args.unselected_fg].value;
    const uintattr_t uns_bg = color_mapping[args.unselected_bg].value;
    const uintattr_t sel_fg = color_mapping[args.selected_fg].value;
    const uintattr_t sel_bg = color_mapping[args.selected_bg].value;

    const int spinner_x_pos = x + spinner_menu->max_option_length + 1;
    //print title
    tb_printf(x, y++, uns_fg, uns_bg, "%s", spinner_menu->menu->title);

    for (int i = 0; i < spinner_menu->menu->option_count; i++) {
        tb_printf(x, y, uns_fg, uns_bg, "%s", spinner_menu->menu->options[i]);
        if (i == spinner_menu->menu->selected_index / 2 && spinner_menu->menu->selected_index % 2 == 0 && args.active) {
            // the left symbol is marked
            tb_printf(spinner_x_pos, y, sel_fg, sel_bg, "%c", spinner_menu->left_symbol);
            tb_printf(spinner_x_pos + 2, y, uns_fg, uns_bg, "%c", spinner_menu->right_symbol);
        } else if (i == spinner_menu->menu->selected_index / 2 && spinner_menu->menu->selected_index % 2 == 1 && args.active) {
            // the right symbol is marked
            tb_printf(spinner_x_pos, y, uns_fg, uns_bg, "%c", spinner_menu->left_symbol);
            tb_printf(spinner_x_pos + 2, y, sel_fg, sel_bg, "%c", spinner_menu->right_symbol);
        } else {
            tb_printf(spinner_x_pos, y, uns_fg, uns_bg, "%c", spinner_menu->left_symbol);
            tb_printf(spinner_x_pos + 2, y, uns_fg, uns_bg, "%c", spinner_menu->right_symbol);
        }
        y += 1;
    }

    tb_printf(x, y + 2, uns_fg, uns_bg, "%s", spinner_menu->menu->tailing_text);
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
    const menu_arg_t default_args = {1, BLACK, WHITE, WHITE, DEFAULT};
    if (args == NULL) {
        return default_args;
    }
    return *args;
}
