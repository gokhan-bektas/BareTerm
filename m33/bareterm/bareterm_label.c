// ============================
// File: bareterm_label.c
// ============================
#include "bareterm_widget.h"
#include <string.h>

// Draw function for label
static void label_draw(bareterm_widget_t *w) {
    bareterm_move_cursor(w->x, w->y);
    bareterm_set_color(w->fg, w->bg, w->style);
    bareterm_puts(w->label);
    bareterm_reset_color();
}

// Labels don’t handle events
static void label_event(bareterm_widget_t *w, const bareterm_event_t *evt) {
    (void)w; (void)evt;
}

void bareterm_label_init(bareterm_widget_t *lbl, int x, int y, const char *text,
                    bareterm_color_t fg, bareterm_color_t bg, bareterm_style_t style) {
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
    bareterm_widget_register(lbl);
}

// Public: update an existing label's text
void bareterm_label_set_text(bareterm_widget_t *lbl, const char *text) {
    lbl->label = text;
    lbl->needs_redraw = true;
}

// Public: update an existing label's color and style
void bareterm_label_set_color(bareterm_widget_t *lbl, bareterm_color_t fg, bareterm_color_t bg, bareterm_style_t style) {
    lbl->fg = fg;
    lbl->bg = bg;
    lbl->style = style;
    lbl->needs_redraw = true;
}