#include "settings_screen.h"
#include "main_screen.h" // Needed to navigate back
#include <bareterm.h>
#include <widget/bareterm_widget.h>
#include <bareterm_screen_manager.h>
#include <stdio.h>

// --- Widgets for this screen ---
static bareterm_widget_t lbl_title;
static bareterm_widget_t btn_back;

// --- Callbacks for this screen ---

// This is the key to navigation. It tells the screen manager
// to switch back to the main screen.
static void on_back_button_click(void *w) {
    bareterm_screen_manager_set_active(main_screen_get());
}

// --- Screen lifecycle functions ---
static void on_settings_screen_enter(bareterm_screen_t *screen) {
    bareterm_draw_box(1, 1, 80, 30, " Settings ");

    bareterm_label_init(&lbl_title,
        28, 10,
        "This is the settings screen.",
        bareterm_WHITE, bareterm_BLACK, bareterm_STYLE_NONE);

    bareterm_button_init(&btn_back,
        30, 15,
        20, 3,
        "[ Back ]",
        on_back_button_click);
}

// --- Screen definition ---
static bareterm_screen_t settings_screen_def = {
    .on_enter = on_settings_screen_enter,
};

bareterm_screen_t* settings_screen_get(void) {
    return &settings_screen_def;
}