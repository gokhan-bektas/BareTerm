// ============================
// File: tui_label.c
// ============================
#include "tui_widget.h"
#include <string.h>

// Draw function for label
static void label_draw(tui_widget_t *w) {
    tui_move_cursor(w->x, w->y);
    tui_puts((const char*)w->user_data);
}

// Labels don’t handle events
static void label_event(tui_widget_t *w, const tui_event_t *evt) {
    (void)w; (void)evt;
}

void tui_label_init(tui_widget_t *lbl, int x, int y, const char *text) {
    lbl->x             = x;
    lbl->y             = y;
    lbl->width         = strlen(text);
    lbl->height        = 1;
    lbl->focused       = false;
    lbl->visible       = true;
    lbl->needs_redraw  = true;
    lbl->draw          = label_draw;
    lbl->handle_event  = label_event;
    lbl->parent        = NULL;
    lbl->next_sibling  = NULL;
    lbl->user_data     = (void*)text;
    tui_widget_register(lbl);
}