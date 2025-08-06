// ============================
// File: tui_modalpopup.c
// ============================
#include "tui_widget.h"
#include "tui_backend.h"
#include <string.h>
#include <zephyr/kernel.h>

// Show a modal dialog in the alternate buffer
void tui_modalpopup_show(const char *title, const char *msg) {
    // 1) Switch into the alternate screen
    tui_backend_puts("\x1B[?1049h");  // DECSET 1049

    // 2) Clear and draw your dialog
    //tui_clear_screen();
    tui_set_color(TUI_WHITE, TUI_BLUE, TUI_STYLE_BOLD);
    tui_draw_box(10, 5, 40, 7, title);
    tui_move_cursor(12, 8);
    tui_puts(msg);
    tui_reset_color();

    // 3) Prompt and wait for any key
    tui_move_cursor(12, 11);
    tui_puts("(press any key to continue)");
    // Simple blocking getchar; adapt to your input system

    k_msleep(2000);


    // 4) Switch back to the main screen
    tui_backend_puts("\x1B[?1049l");  // DECRST 1049
}