// ============================
// File: bareterm_modalpopup.c
// ============================
#include "bareterm_widget.h"
#include "../bareterm_backend.h"
#include <string.h>
//#include <zephyr/kernel.h>

// Show a modal dialog in the alternate buffer
void bareterm_modalpopup_show(const char *title, const char *msg) {
    // 1) Switch into the alternate screen
    bareterm_backend_puts("\x1B[?1049h");  // DECSET 1049

    // 2) Clear and draw your dialog
    //bareterm_clear_screen();
    bareterm_set_color(bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
    bareterm_draw_box(10, 5, 40, 7, title);
    bareterm_move_cursor(12, 8);
    bareterm_puts(msg);
    bareterm_reset_color();

    // 3) Prompt and wait for any key
    bareterm_move_cursor(12, 11);
    bareterm_puts("(press any key to continue)");
    // Simple blocking getchar; adapt to your input system

    ///k_msleep(2000);


    // 4) Switch back to the main screen
    bareterm_backend_puts("\x1B[?1049l");  // DECRST 1049
}