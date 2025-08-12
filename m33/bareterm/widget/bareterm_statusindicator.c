// ============================
// File: bareterm_statusindicator.c
// ============================
#include "bareterm_widget.h"
#include <string.h>

// Unicode for the indicator: ● (U+25CF), ■ (U+25A0)
static const char *INDICATOR_SHAPE = "\xE2\x97\x8F"; // ●

static void statusindicator_draw(bareterm_widget_t *w) {
    bareterm_move_cursor(w->x, w->y);
    
    // Draw the indicator shape with the status color
    bareterm_set_color(w->fg, w->bg, w->style); // Use fg for the status color
    bareterm_puts(INDICATOR_SHAPE);
    bareterm_reset_color();

    // Draw the label next to it
    if (w->label) {
        bareterm_puts(" ");
        bareterm_puts(w->label);
    }
}

// Status indicators are non-interactive
static void statusindicator_event(bareterm_widget_t *w, const bareterm_event_t *evt) {
    (void)w; (void)evt;
}

void bareterm_statusindicator_init(bareterm_widget_t *si,
                              int x, int y,
                              const char *label,
                              bareterm_color_t initial_color) {
    si->x = x;
    si->y = y;
    si->label = label;
    si->width = 1 + (label ? (1 + strlen(label)) : 0); // shape + space + label
    si->height = 1;
    si->fg = initial_color;
    si->bg = bareterm_BLACK;
    si->style = bareterm_STYLE_NONE;
    si->visible = true;
    si->needs_redraw = true;
    si->draw = statusindicator_draw;
    si->handle_event = statusindicator_event;
    bareterm_widget_register(si);
}

void bareterm_statusindicator_set_color(bareterm_widget_t *si, bareterm_color_t color) {
    if (si->fg != color) {
        si->fg = color;
        si->needs_redraw = true;
    }
}