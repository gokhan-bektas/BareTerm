#include <bareterm_screen.h>
#include <bareterm.h>
#include <widget/bareterm_widget.h>
#include <stdio.h>
#include <string.h>
#include <bareterm_screen_manager.h>
#include "settings_screen.h"
#include "main_screen.h"

// --- All the static widget variables from main.c go here ---
static bareterm_widget_t btn1, btn2;
static bareterm_widget_t memo;
static bareterm_widget_t lbl1;
static bareterm_widget_t lbl2;
static bareterm_widget_t bar;
static bareterm_widget_t cb1, cb2;
static bareterm_widget_t rg;
static const char *choices[] = { "SPI", "I2C", "UART", "I2C"};
static bareterm_widget_t status_led;
static bareterm_widget_t listbox;
static const char *list_items[] = {
    "First Item", "Second Item", "A Third, Longer Item", "Item Four",
    "Fifth", "Sixth Sense", "Seventh Heaven", "Eighth Wonder",
    "Ninth Life", "Tenth Planet"
};
static bareterm_widget_t input;
static char buf[64] = "";

// --- All the callback functions from main.c go here ---
void on_submit(bareterm_widget_t *ti) {
    // Copy out and display
    char out[64];
    bareterm_textinput_get_text(ti, out, sizeof(out));
    bareterm_move_cursor(1, 20);
    bareterm_printf("You entered: %s", out);
}

static void on_button_click1(void *w) {
    bareterm_label_set_text(&lbl1, "Clicked Button 1!");
    bareterm_label_set_color(&lbl1, bareterm_YELLOW, bareterm_BLUE, bareterm_STYLE_BOLD);
	bareterm_progressbar_set_value(&bar, bar.value - 1);
    static int status_idx = 0;
    bareterm_color_t colors[] = { bareterm_GREEN, bareterm_YELLOW, bareterm_RED, bareterm_WHITE };
    status_idx = (status_idx + 1) % 4;
    bareterm_statusindicator_set_color(&status_led, colors[status_idx]);
}

static void on_button_click2(void *w) {
    // Navigate to the settings screen
    bareterm_screen_manager_set_active(settings_screen_get());
}

static void on_toggle(bareterm_widget_t *cb, unsigned char state) {
    bareterm_move_cursor(10, 20);
    bareterm_printf("Checkbox is now %s   ", state ? "☑️" : "☐");
	if (state) {
		bareterm_label_set_text(&lbl2, "Checkbox enabled");
		bareterm_modalpopup_show("Checkbox Enabled", "You have enabled the checkbox feature.");
	} else {
		bareterm_label_set_text(&lbl2, "Checkbox disabled");
        bareterm_modalpopup_show("Checkbox Disabled", "The feature has been turned off.");
	}
}

void on_choice_change(bareterm_widget_t *w, int sel) {
    bareterm_move_cursor(4, 18);
    bareterm_printf("Selected: %s   ", choices[sel]);
}

void on_list_select(bareterm_widget_t *w, int sel) {
    bareterm_move_cursor(1, 22);
    bareterm_printf("Listbox selected: %-20s", list_items[sel]);
}

// This function is called when the screen becomes active.
// It is responsible for creating all the widgets for this screen.
static void on_main_screen_enter(bareterm_screen_t *screen) {
    // This is where all the bareterm_*_init() calls from main() go.
    bareterm_draw_box(1, 1, 80, 30, " MAX32657 EVKit Test Tool v0.1.0");
	bareterm_label_init(&lbl1, 15, 4, "Welcome", bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
	bareterm_label_init(&lbl2, 14, 24, "Click somewhere to start", bareterm_YELLOW, bareterm_BLUE, bareterm_STYLE_BOLD);
    bareterm_button_init(&btn1, 4, 6, 20, 3, "[ Button1 ]", on_button_click1);
    bareterm_button_init(&btn2, 26, 6, 20, 3, "[ Settings ]", on_button_click2);
	bareterm_textbox_init(&memo, 3, 10, 36, 5, "To start tests please follow the instructions below: Here are the instructions. Under construction...", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);
    bareterm_progressbar_init(&bar, 4,  15, 38, 1, 30, 100, bareterm_GREEN, bareterm_BLACK, bareterm_STYLE_NONE);
    bareterm_checkbox_init(&cb1, 12, 27, "Enable feature 1", false, on_toggle);
    bareterm_checkbox_init(&cb2, 12, 28, "Enable feature 2", false, on_toggle);
    bareterm_radiogroup_init(&rg, 4, 19, choices, 4, 1, on_choice_change);
    bareterm_statusindicator_init(&status_led, 40, 17, "System Status", bareterm_GREEN);
    bareterm_listbox_init(&listbox, 52, 2, 25, 12, list_items, 10, on_list_select);
    
    // The text input 
    // bareterm_textinput_init(&input, ...);
}

// This is the definition of our screen.
static bareterm_screen_t main_screen_def = {
    .on_enter = on_main_screen_enter,
    .on_leave = NULL, // No special cleanup needed
    .user_data = NULL,
};

// This is the public function to get a pointer to our screen.
bareterm_screen_t* main_screen_get(void) {
    return &main_screen_def;
}
