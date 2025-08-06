// ============================
// File: tui_checkbox.c
// ============================
#include "tui_widget.h"
#include <string.h>

// ============================
// Checkbox draw & event
// ============================
static void checkbox_draw(tui_widget_t *w) {
    // ▢ = U+25A2 white square, ▣ = U+25A3 checked
    const char *BOX_UNCHECK = "\xE2\x96\xA2"; // ▢
    const char *BOX_CHECK   = "\xE2\x96\xA3"; // ▣

    tui_move_cursor(w->x, w->y);
    // draw box
    tui_set_color(w->fg, w->bg, w->style);
    tui_puts(w->checked ? BOX_CHECK : BOX_UNCHECK);
    // space + label
    tui_puts(" ");
    tui_puts(w->label);
    tui_reset_color();
}

static void checkbox_event(tui_widget_t *w, const tui_event_t *evt) {
    unsigned char inside = 0;
    if (evt->type == TUI_EVT_MOUSE && evt->mouse.pressed) {
        int mx = evt->mouse.x, my = evt->mouse.y;
        inside = (mx >= w->x && mx < w->x + 1  // only the box is clickable
               && my >= w->y && my < w->y + 1);
    }
    else if (evt->type == TUI_EVT_KEY && evt->key.code == TUI_KEY_ENTER) {
        inside = w->focused;
    }

    if (inside) {
        // toggle state
        w->checked = !w->checked;
        // callback
        if (w->toggle_cb) w->toggle_cb(w, w->checked);
        w->needs_redraw = 1;
    }
}

// ============================
// Public API: checkbox init & access
// ============================
void tui_checkbox_init(tui_widget_t *cb,
                       int x, int y,
                       const char *label,
                       unsigned char initial,
                       void (*on_toggle)(tui_widget_t *cb, unsigned char new_state))
{
    cb->x            = x;
    cb->y            = y;
    cb->width        = 1 + 1 + strlen(label); // box + space + label
    cb->height       = 1;
    cb->label        = label;
    cb->checked      = initial;
    cb->toggle_cb    = on_toggle;
    // default colors (you can override with set_color later)
    cb->fg           = TUI_WHITE;
    cb->bg           = TUI_BLACK;
    cb->style        = TUI_STYLE_NONE;
    cb->visible      = 1;
    cb->needs_redraw = 1;
    // hook up draw & event
    cb->draw         = checkbox_draw;
    cb->handle_event = checkbox_event;
    // register
    tui_widget_register(cb);
}

void tui_checkbox_set_checked(tui_widget_t *cb, unsigned char checked) {
    cb->checked      = checked;
    cb->needs_redraw = 1;
}

unsigned char tui_checkbox_is_checked(const tui_widget_t *cb) {
    return cb->checked;
}