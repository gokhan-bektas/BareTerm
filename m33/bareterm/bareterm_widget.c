// ============================
// File: bareterm_widget.c
// ============================
#include "bareterm_widget.h"
#include <string.h>

// Internal linked list head and focus tracking
static bareterm_widget_t *widget_list = NULL;
static bareterm_widget_t *focused_widget = NULL;

// Register widget: add to list, set initial focus
void bareterm_widget_register(bareterm_widget_t *w) {
    w->next_sibling = widget_list;
    widget_list = w;
    w->parent = NULL;
    w->needs_redraw = true;
    if (!focused_widget) {
        focused_widget = w;
        w->focused = true;
    } else {
        w->focused = false;
    }
}

// Draw all widgets in the order registered
void bareterm_draw_all_widgets(void) {
    for (bareterm_widget_t *w = widget_list; w; w = w->next_sibling) {
        if (w->visible && w->draw) {
            w->draw(w);
            w->needs_redraw = false;
        }
    }
}

// Dispatch event to appropriate widget (mouse hit-test + keyboard focus)
void bareterm_dispatch_event(const bareterm_event_t *evt) {
    // Mouse events: hit-test all widgets
    if (evt->type == bareterm_EVT_MOUSE) {
        for (bareterm_widget_t *w = widget_list; w; w = w->next_sibling) {
            if (!w->visible) continue;
            int mx = evt->mouse.x;
            int my = evt->mouse.y;
            if (mx >= w->x && mx < w->x + w->width &&
                my >= w->y && my < w->y + w->height) {
                // change focus
                if (focused_widget != w) {
                    if (focused_widget) focused_widget->focused = false;
                    focused_widget = w;
                    w->focused = true;
                }
                // send event
                if (w->handle_event) w->handle_event(w, evt);
                w->needs_redraw = true;
                return;
            }
        }
    }
    // Keyboard and clicks outside go to focused widget
    if (focused_widget && focused_widget->handle_event) {
        focused_widget->handle_event(focused_widget, evt);
        focused_widget->needs_redraw = true;
    }
}

// Button draw and event handlers
static void button_draw(bareterm_widget_t *w) {
    // Draw border
    bareterm_draw_box(w->x, w->y, w->width, w->height, NULL);
    // Center label
    int len = strlen(w->label);
    int cx = w->x + (w->width - len) / 2;
    int cy = w->y + w->height / 2;
    bareterm_move_cursor(cx, cy);
    if (w->focused) bareterm_set_color(bareterm_BLACK, bareterm_WHITE, bareterm_STYLE_BOLD);
    bareterm_puts(w->label);
    if (w->focused) bareterm_reset_color();
}

static void button_event(bareterm_widget_t *w, const bareterm_event_t *evt) {
    // Handle Enter key or mouse click
    if (evt->type == bareterm_EVT_KEY && evt->key.code == bareterm_KEY_ENTER) {
        void (*cb)(void *) = w->click_cb;
        if (cb) cb(w);
    }
    if (evt->type == bareterm_EVT_MOUSE && evt->mouse.pressed) {
        int mx = evt->mouse.x;
        int my = evt->mouse.y;
        if (mx >= w->x && mx < w->x + w->width &&
            my >= w->y && my < w->y + w->height) {
            void (*cb)(void *) = w->click_cb;
            if (cb) cb(w);
        }
    }
}

// Public: initialize a button widget
void bareterm_button_init(bareterm_widget_t *btn,
                     int x, int y, int w, int h,
                     const char *label,
                     void (*on_click)(void *)) {
    // Set geometry
    btn->x = x;
    btn->y = y;
    btn->width = w;
    btn->height = h;

    // Widget state
    btn->visible = true;
    btn->needs_redraw = true;

    // Button-specific data
    btn->label = label;
    btn->click_cb = on_click;

    // Assign draw and event handlers
    btn->draw = button_draw;
    btn->handle_event = button_event;

    // Register in widget system
    bareterm_widget_register(btn);
}




