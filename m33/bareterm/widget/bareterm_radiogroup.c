// ============================
// File: bareterm_radiogroup.c
// ============================
#include "bareterm_widget.h"
#include <string.h>

// ============================
// Radio‐button group draw & event
// ============================
static void radiogroup_draw(bareterm_widget_t *w) {
    // Unicode bullets: ● (U+25CF), ○ (U+25CB)
    const char *SEL   = "\xE2\x97\x8F"; // ●
    const char *UNSEL = "\xE2\x97\x8B"; // ○

    for (int i = 0; i < w->rg_count; i++) {
        bareterm_move_cursor(w->x, w->y + i);
        // draw bullet + space
        bareterm_set_color(w->fg, w->bg, w->style);
        bareterm_puts((w->rg_selected == i) ? SEL : UNSEL);
        bareterm_puts(" ");
        bareterm_puts(w->rg_labels[i]);
        bareterm_reset_color();
    }
}

static void radiogroup_event(bareterm_widget_t *w, const bareterm_event_t *evt) {
    int old = w->rg_selected;

    //printf("event %d %d\n", evt->type, evt->key.code);

    if ((evt->type == bareterm_EVT_KEY) || (evt->type == bareterm_EVT_ARROW)) {
        switch (evt->key.code) {
            case bareterm_KEY_UP:
                if (w->rg_selected > 0) w->rg_selected--;
                break;
            case bareterm_KEY_DOWN:
                if (w->rg_selected < w->rg_count - 1) w->rg_selected++;
                break;
            case bareterm_KEY_ENTER:
                // Enter confirms current selection—trigger callback
                if (w->rg_change_cb) w->rg_change_cb(w, w->rg_selected);
                break;
            default:
                return;
        }
    }
    else if (evt->type == bareterm_EVT_MOUSE && evt->mouse.pressed) {
        int mx = evt->mouse.x, my = evt->mouse.y;
        // if click falls on one of the rows
        if (mx >= w->x && mx < w->x + 2 + /*max label length?*/  w->width &&
            my >= w->y && my < w->y + w->rg_count) {
            int idx = my - w->y;
            if (idx >= 0 && idx < w->rg_count) {
                w->rg_selected = idx;
                if (w->rg_change_cb) w->rg_change_cb(w, idx);
            }
        }
    }

    if (w->rg_selected != old) {
        w->needs_redraw = 1;
    }
}

// ============================
// Public API: init & access
// ============================
void bareterm_radiogroup_init(bareterm_widget_t *rg,
                         int x, int y,
                         const char **labels,
                         int count,
                         int initial,
                         void (*on_change)(bareterm_widget_t *rg, int selected))
{
    rg->x            = x;
    rg->y            = y;
    rg->width        = 2 + 1;           // bullet + space; labels draw right after
    for (int i = 0; i < count; i++) {
        int len = strlen(labels[i]);
        if (rg->width < 2 + len) rg->width = 2 + len;
    }
    rg->height       = count;
    rg->rg_labels    = labels;
    rg->rg_count     = count;
    rg->rg_selected  = (initial >= 0 && initial < count) ? initial : 0;
    rg->rg_change_cb = on_change;

    // default styling
    rg->fg           = bareterm_WHITE;
    rg->bg           = bareterm_BLACK;
    rg->style        = bareterm_STYLE_NONE;

    rg->visible      = 1;
    rg->needs_redraw = 1;
    rg->draw         = radiogroup_draw;
    rg->handle_event = radiogroup_event;

    bareterm_widget_register(rg);
}

void bareterm_radiogroup_set_selected(bareterm_widget_t *rg, int selected) {
    if (selected < 0) selected = 0;
    if (selected >= rg->rg_count) selected = rg->rg_count - 1;
    if (rg->rg_selected != selected) {
        rg->rg_selected = selected;
        rg->needs_redraw = 1;
        if (rg->rg_change_cb) rg->rg_change_cb(rg, selected);
    }
}

int bareterm_radiogroup_get_selected(const bareterm_widget_t *rg) {
    return rg->rg_selected;
}
