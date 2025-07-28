// ============================
// File: tui_label.c
// ============================
#include "tui_widget.h"
#include <string.h>


// ============================
// TextBox draw and event handlers
// ============================
static void textbox_draw(tui_widget_t *w) {
    // Draw multiline text with word wrap
    const char *text = w->label;
    int maxcols = w->width;
    int row = 0;
    const char *p = text;
    while (*p && row < w->height) {
        // Skip leading spaces
        while (*p == ' ') p++;
        // Start line
        int col = 0;
        const char *line_start = p;
        int last_space = -1;
        int len = 0;
        while (*p && len < maxcols) {
            if (*p == ' ') last_space = len;
            p++;
            len++;
        }
        int line_len = len;
        if (*p && last_space >= 0) {
            // Wrap at last space
            p = line_start + last_space;
            line_len = last_space;
        }
        // Draw this line
        tui_move_cursor(w->x, w->y + row);
        tui_set_color(w->fg, w->bg, w->style);
        for (int i = 0; i < line_len; i++) {
            tui_puts((char[]){line_start[i], 0});
        }
        tui_reset_color();
        row++;
        // Skip spaces to start next line
        while (*p == ' ') p++;
    }
}

static void textbox_event(tui_widget_t *w, const tui_event_t *evt) {
    (void)w; (void)evt; // read-only widget
}

// Public: initialize a textbox widget
void tui_textbox_init(tui_widget_t *tb,
                      int x, int y, int w, int h,
                      const char *text,
                      tui_color_t fg, tui_color_t bg, tui_style_t style) {
    tb->x = x;
    tb->y = y;
    tb->width = w;
    tb->height = h;
    tb->label = text;
    tb->fg = fg;
    tb->bg = bg;
    tb->style = style;
    tb->visible = true;
    tb->needs_redraw = true;
    tb->draw = textbox_draw;
    tb->handle_event = textbox_event;
    tui_widget_register(tb);
}

// Public: update textbox content at runtime
void tui_textbox_set_text(tui_widget_t *tb, const char *text) {
    tb->label = text;
    tb->needs_redraw = true;
}

// Public: update textbox color and style at runtime
void tui_textbox_set_color(tui_widget_t *tb, tui_color_t fg, tui_color_t bg, tui_style_t style) {
    tb->fg = fg;
    tb->bg = bg;
    tb->style = style;
    tb->needs_redraw = true;
}