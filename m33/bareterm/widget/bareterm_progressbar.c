// ============================
// File: bareterm_progressbar.c
// ============================
#include "bareterm_widget.h"
#include <string.h>
#include <stdio.h> 

static void progressbar_draw(bareterm_widget_t *w) {
    // Unicode blocks
    const char *FILLED = "\xE2\x96\x88"; // █
    const char *EMPTY_BLOCK  = "\xE2\x96\x91"; // ░

    // Total width includes brackets: "[…]"
    int total    = w->width;
    int inner    = total - 2;
    if (inner < 1) inner = 1;

    // How many cells to fill
    int filled = (int)((long)w->value * inner / w->max);
    if (filled < 0)       filled = 0;
    if (filled > inner)   filled = inner;

    // Draw the bar
    bareterm_move_cursor(w->x, w->y);
    bareterm_set_color(w->fg, w->bg, w->style);
    bareterm_puts("[");
    for (int i = 0; i < filled;   i++) bareterm_puts(FILLED);
    for (int i = filled; i < inner; i++) bareterm_puts(EMPTY_BLOCK);
    bareterm_puts("]");
    bareterm_reset_color();

    // Draw the percentage to the right
    char pct[8];
    int percent = (int)(100 * w->value / w->max);
    snprintf(pct, sizeof(pct), " %3d%%", percent);

    // Move cursor just past the closing bracket
    bareterm_move_cursor(w->x + total, w->y);
    bareterm_puts(pct);
}

// No events to handle (read–only bar)
static void progressbar_event(bareterm_widget_t *w, const bareterm_event_t *evt) {
    (void)w; (void)evt;
}

// Public API: initialize a progress bar
void bareterm_progressbar_init(bareterm_widget_t *pb,
                          int x, int y, int w, int h,
                          int value, int max,
                          bareterm_color_t fg, bareterm_color_t bg, bareterm_style_t style)
{
    pb->x           = x;
    pb->y           = y;
    pb->width       = w;
    pb->height      = h;
    pb->value       = value;
    pb->max         = max;
    pb->fg          = fg;
    pb->bg          = bg;
    pb->style       = style;
    pb->visible     = true;
    pb->needs_redraw= true;

    pb->draw        = progressbar_draw;
    pb->handle_event = progressbar_event;

    bareterm_widget_register(pb);
}

// Public API: update its value at runtime
void bareterm_progressbar_set_value(bareterm_widget_t *pb, int value) {
    if (value < 0)      value = 0;
    else if (value > pb->max) value = pb->max;
    pb->value        = value;
    pb->needs_redraw = true;
}
