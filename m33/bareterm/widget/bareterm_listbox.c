// ============================
// File: bareterm_listbox.c
// ============================
#include "bareterm_widget.h"
#include <string.h>

// ============================
// Listbox draw & event
// ============================

static void listbox_draw(bareterm_widget_t *w) {
    bareterm_draw_box(w->x, w->y, w->width, w->height, NULL);

    for (int i = 0; i < w->height - 2; i++) {
        int item_idx = w->lb_top + i;
        if (item_idx >= w->lb_count) break;

        bareterm_move_cursor(w->x + 1, w->y + 1 + i);

        bool is_selected = (item_idx == w->lb_selected);
        if (is_selected) {
            bareterm_set_color(bareterm_BLACK, bareterm_WHITE, bareterm_STYLE_NONE);
        }

        // Print item text, padded to fill width
        const char *text = w->lb_items[item_idx];
        int len = strlen(text);
        bareterm_puts(text);
        for (int j = len; j < w->width - 2; j++) {
            bareterm_puts(" ");
        }

        if (is_selected) {
            bareterm_reset_color();
        }
    }
    
    // Optional: Draw a scrollbar
    if (w->lb_count > w->height - 2) {
        int scroll_h = w->height - 2;
        int thumb_h = (scroll_h * scroll_h) / w->lb_count;
        if (thumb_h < 1) thumb_h = 1;
        int thumb_y = (w->lb_top * (scroll_h - thumb_h)) / (w->lb_count - scroll_h);

        for(int i=0; i < scroll_h; i++) {
            bareterm_move_cursor(w->x + w->width - 2, w->y + 1 + i);
            if (i >= thumb_y && i < thumb_y + thumb_h) {
                bareterm_puts("\xE2\x96\x88"); // █
            } else {
                bareterm_puts("\xE2\x96\x91"); // ░
            }
        }
    }
}

static void listbox_event(bareterm_widget_t *w, const bareterm_event_t *evt) {
    int old_selected = w->lb_selected;

    if (evt->type == bareterm_EVT_ARROW) {
        if (evt->key.code == bareterm_KEY_UP && w->lb_selected > 0) {
            w->lb_selected--;
        } else if (evt->key.code == bareterm_KEY_DOWN && w->lb_selected < w->lb_count - 1) {
            w->lb_selected++;
        }
    } else if (evt->type == bareterm_EVT_KEY && evt->key.code == bareterm_KEY_ENTER) {
        if (w->lb_select_cb) w->lb_select_cb(w, w->lb_selected);
        return;
    } else if (evt->type == bareterm_EVT_MOUSE && evt->mouse.pressed) {
        int my = evt->mouse.y;
        if (my > w->y && my < w->y + w->height - 1) {
            int clicked_idx = w->lb_top + (my - (w->y + 1));
            if (clicked_idx >= 0 && clicked_idx < w->lb_count) {
                w->lb_selected = clicked_idx;
                if (w->lb_select_cb) w->lb_select_cb(w, w->lb_selected);
            }
        }
    }

    // Update scroll position
    int view_height = w->height - 2;
    if (w->lb_selected < w->lb_top) {
        w->lb_top = w->lb_selected;
    } else if (w->lb_selected >= w->lb_top + view_height) {
        w->lb_top = w->lb_selected - view_height + 1;
    }

    if (w->lb_selected != old_selected) w->needs_redraw = true;
}

void bareterm_listbox_init(bareterm_widget_t *lb, int x, int y, int w, int h,
                      const char **items, int count,
                      void (*on_select)(bareterm_widget_t *lb, int selected)) {
    lb->x = x; lb->y = y; lb->width = w; lb->height = h;

    lb->lb_items = items;
    lb->lb_count = count;
    lb->lb_selected = 0;
    lb->lb_top = 0;
    lb->lb_select_cb = on_select;

    lb->visible = true; lb->needs_redraw = true;
    lb->draw = listbox_draw; lb->handle_event = listbox_event;
    bareterm_widget_register(lb);
}