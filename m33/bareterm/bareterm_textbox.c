// ============================
// File: bareterm_label.c
// ============================
#include "bareterm_widget.h"
#include <string.h>


// ============================
// TextBox draw and event handlers
// ============================
void textbox_draw(bareterm_widget_t *w) {
    // Draw multiline text with word wrap
    const char *text = w->label;
    int maxcols = w->width;
    int row = 0;
    const char *p = text;
    while (*p && row < w->height) {
        // Skip leading spaces
        while (*p == ' ') p++;
        // Start line
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
        bareterm_move_cursor(w->x, w->y + row);
        bareterm_set_color(w->fg, w->bg, w->style);
        for (int i = 0; i < line_len; i++) {
            bareterm_puts((char[]){line_start[i], 0});
        }
        bareterm_reset_color();
        row++;
        // Skip spaces to start next line
        while (*p == ' ') p++;
    }
}

void textbox_event(bareterm_widget_t *w, const bareterm_event_t *evt) {
    (void)w; (void)evt; // read-only widget
}

// Public: initialize a textbox widget
void bareterm_textbox_init(bareterm_widget_t *tb,
                      int x, int y, int w, int h,
                      const char *text,
                      bareterm_color_t fg, bareterm_color_t bg, bareterm_style_t style) {
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
    bareterm_widget_register(tb);
}

// Public: update textbox content at runtime
void bareterm_textbox_set_text(bareterm_widget_t *tb, const char *text) {
    tb->label = text;
    tb->needs_redraw = true;
}

// Public: update textbox color and style at runtime
void bareterm_textbox_set_color(bareterm_widget_t *tb, bareterm_color_t fg, bareterm_color_t bg, bareterm_style_t style) {
    tb->fg = fg;
    tb->bg = bg;
    tb->style = style;
    tb->needs_redraw = true;
}