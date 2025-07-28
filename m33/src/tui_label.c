// ============================
// File: tui_label.c
// ============================
#include "tui_widget.h"
#include <string.h>

// Draw function for label
static void label_draw(tui_widget_t *w) {
    tui_move_cursor(w->x, w->y);
    tui_set_color(w->fg, w->bg, w->style);
    tui_puts(w->label);
    tui_reset_color();
}

// Labels don’t handle events
static void label_event(tui_widget_t *w, const tui_event_t *evt) {
    (void)w; (void)evt;
}

void tui_label_init(tui_widget_t *lbl, int x, int y, const char *text,
                    tui_color_t fg, tui_color_t bg, tui_style_t style) {
    lbl->x = x;
    lbl->y = y;
    lbl->width = strlen(text);
    lbl->height = 1;
    lbl->visible = true;
    lbl->needs_redraw = true;
    lbl->label = text;
    lbl->fg    = fg;
    lbl->bg    = bg;
    lbl->style = style;
    lbl->draw  = label_draw;
    lbl->handle_event = label_event;
    tui_widget_register(lbl);
}

// Public: update an existing label's text
void tui_label_set_text(tui_widget_t *lbl, const char *text) {
    lbl->label = text;
    lbl->needs_redraw = true;
}

// Public: update an existing label's color and style
void tui_label_set_color(tui_widget_t *lbl, tui_color_t fg, tui_color_t bg, tui_style_t style) {
    lbl->fg = fg;
    lbl->bg = bg;
    lbl->style = style;
    lbl->needs_redraw = true;
}