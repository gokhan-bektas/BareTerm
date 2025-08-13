#include "bareterm_main_screen.h"
#include <bareterm.h>
#include <widget/bareterm_widget.h>
#include <stdio.h>
#include <string.h>

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
    bareterm_label_set_text(&lbl1, "Clicked Button 2!");
    bareterm_label_set_color(&lbl1, bareterm_YELLOW, bareterm_GREEN, bareterm_STYLE_BOLD);
	bareterm_progressbar_set_value(&bar, bar.value + 1);
}

static void on_toggle(bareterm_widget_t *cb, unsigned char state) {
    bareterm_move_cursor(10, 20);
    bareterm_printf("Checkbox is now %s   ", state ? "☑️" : "☐");
	if (state) {
		bareterm_label_set_text(&lbl2, "Checkbox enabled");
		bareterm_modalpopup_show("Checkbox Enabled", "You have enabled the checkbox feature.");
	} else {
		bareterm_label_set_