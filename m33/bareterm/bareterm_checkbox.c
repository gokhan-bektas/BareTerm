// ============================
// File: bareterm_checkbox.c
// ============================
#include "bareterm_widget.h"
#include <string.h>

// ============================
// Checkbox draw & event
// ============================
static void checkbox_draw(bareterm_widget_t *w) {
    // ▢ = U+25A2 white square, ▣ = U+25A3 checked
    const char *BOX_UNCHECK = "\xE2\x96\xA2"; // ▢
    const char *BOX_CHECK   = "\xE2\x96\xA3"; // ▣

    bareterm_move_cursor(w->x, w->y);
    // draw box
    bareterm_set_color(w->fg, w->bg, w->style);
    bareterm_puts(w->checked ? BOX_CHECK : BOX_UNCHECK);
    // space + label
    bareterm_puts(" ");
    bareterm_puts(w->label);
    bareterm_reset_color();
}

static void checkbox_event(bareterm_widget_t *w, const bareterm_event_t *evt) {
    unsigned char inside = 0;
    if (evt->type == bareterm_EVT_MOUSE && evt->mouse.pressed) {
        int mx = evt->mouse.x, my = evt->mouse.y;
        inside = (mx >= w->x && mx < w->x + 1  // only the box is clickable
               && my >= w->y && my < w->y + 1);
    }
    else if (evt->type == bareterm_EVT_KEY && evt->key.code == bareterm_KEY_ENTER) {
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
void bareterm_checkbox_init(bareterm_widget_t *cb,
                       int x, int y,
                       const char *label,
                       unsigned char initial,
                       void (*on_toggle)(bareterm_widget_t *cb, unsigned char new_state))
{
    cb->x            = x;
    cb->y            = y;
    cb->width        = 1 + 1 + strlen(label); // box + space + label
    cb->height       = 1;
    cb->label        = label;
    cb->checked      = initial;
    cb->toggle_cb    = on_toggle;
    // default colors (you can override with set_color later)
    cb->fg           = bareterm_WHITE;
    cb->bg           = bareterm_BLACK;
    cb->style        = bareterm_STYLE_NONE;
    cb->visible      = 1;
    cb->needs_redraw = 1;
    // hook up draw & event
    cb->draw         = checkbox_draw;
    cb->handle_event = checkbox_event;
    // register
    bareterm_widget_register(cb);
}

void bareterm_checkbox_set_checked(bareterm_widget_t *cb, unsigned char checked) {
    cb->checked      = checked;
    cb->needs_redraw = 1;
}

unsigned char bareterm_checkbox_is_checked(const bareterm_widget_t *cb) {
    return cb->checked;
}