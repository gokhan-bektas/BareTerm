// ============================
// Text‐Input draw & event
// ============================
#include <string.h>   // for memcpy, memmove, strlen
#include "tui_widget.h"
#include <string.h>

static void textinput_draw(tui_widget_t *w) {
    // 1) Draw a box around the field
    //    x-1,y-1 ┌──…──┐
    //      y    │ text │
    //    y+1   └──…──┘
    tui_draw_box(w->x - 1, w->y - 1, w->width + 2, 3, NULL);

    // 2) Fill the interior with the background color
    for (int col = 0; col < w->width; col++) {
        tui_move_cursor(w->x + col, w->y);
        tui_set_color(w->bg, w->bg, TUI_STYLE_NONE);
        tui_puts(" ");
    }
    tui_reset_color();

    // 3) Render your text (as before)
    tui_move_cursor(w->x, w->y);
    tui_set_color(w->fg, w->bg, w->style);
    int start = w->txt_offset;
    int end   = start + w->width;
    for (int i = start; i < end; i++) {
        char ch = (i < w->txt_length) ? w->txtbuf[i] : ' ';
        tui_puts((char[]){ ch, 0 });
    }
    tui_reset_color();

    // 4) Position the cursor if focused
    if (w->focused) {
        int cx = w->x + (w->cursor_pos - w->txt_offset);
        if (cx >= w->x && cx < w->x + w->width) {
            tui_move_cursor(cx, w->y);
        }
    }
}


static void textinput_event(tui_widget_t *w, const tui_event_t *evt) {
    if (evt->type == TUI_EVT_KEY || evt->type == TUI_EVT_ARROW) {
        int code = (evt->type == TUI_EVT_ARROW)
                   ? evt->key.code
                   : evt->key.ch;

        switch (code) {
            case TUI_KEY_LEFT:
                if (w->cursor_pos > 0) {
                    w->cursor_pos--;
                }
                break;

            case TUI_KEY_RIGHT:
                if (w->cursor_pos < w->txt_length) {
                    w->cursor_pos++;
                }
                break;

            case '\b':      // Backspace
            case 0x7F:      // DEL
                if (w->cursor_pos > 0) {
                    // Remove char before cursor
                    memmove(
                        &w->txtbuf[w->cursor_pos - 1],
                        &w->txtbuf[w->cursor_pos],
                        w->txt_length - w->cursor_pos + 1
                    );
                    w->cursor_pos--;
                    w->txt_length--;
                }
                break;

            case TUI_KEY_ENTER:
                if (w->submit_cb) {
                    w->submit_cb(w);
                }
                return;  // don’t fall through to redraw

            default:
                // Printable ASCII?
                if (evt->type == TUI_EVT_KEY
                 && evt->key.ch >= 32 && evt->key.ch <= 126
                 && w->txt_length < w->txt_capacity - 1) {
                    // Insert character at cursor
                    memmove(
                        &w->txtbuf[w->cursor_pos + 1],
                        &w->txtbuf[w->cursor_pos],
                        w->txt_length - w->cursor_pos + 1
                    );
                    w->txtbuf[w->cursor_pos] = evt->key.ch;
                    w->cursor_pos++;
                    w->txt_length++;
                }
                break;
        }

        // Re‐calculate scrolling window
        if (w->cursor_pos < w->txt_offset) {
            w->txt_offset = w->cursor_pos;
        } else if (w->cursor_pos >= w->txt_offset + w->width) {
            w->txt_offset = w->cursor_pos - w->width + 1;
        }

        w->needs_redraw = true;
    }
    else if (evt->type == TUI_EVT_MOUSE && evt->mouse.pressed) {
        // (unchanged mouse‐click logic)
        int mx = evt->mouse.x - w->x;
        if (mx < 0) mx = 0;
        if (mx > w->width) mx = w->width;
        int pos = w->txt_offset + mx;
        if (pos < 0) pos = 0;
        if (pos > w->txt_length) pos = w->txt_length;
        w->cursor_pos = pos;

        // Adjust scroll again
        if (w->cursor_pos < w->txt_offset)
            w->txt_offset = w->cursor_pos;
        else if (w->cursor_pos >= w->txt_offset + w->width)
            w->txt_offset = w->cursor_pos - w->width + 1;

        w->needs_redraw = true;
    }
}



// ============================
// Public API: TextInput init & access
// ============================
void tui_textinput_init(tui_widget_t *ti,
                        int x, int y, int w,
                        char *buffer, int capacity,
                        void (*on_submit)(tui_widget_t *ti))
{
    ti->x = x;  ti->y = y;
    ti->width  = w;
    ti->height = 1;

    ti->txtbuf      = buffer;
    ti->txt_capacity= capacity;
    ti->txt_length  = strlen(buffer);
    ti->cursor_pos  = ti->txt_length;
    ti->txt_offset  = 0;
    ti->submit_cb   = on_submit;

    // default styling
    ti->fg = TUI_WHITE;
    ti->bg = TUI_BLACK;
    ti->style = TUI_STYLE_NONE;

    ti->visible = true;
    ti->needs_redraw = true;
    ti->draw = textinput_draw;
    ti->handle_event = textinput_event;

    tui_widget_register(ti);
}

void tui_textinput_set_text(tui_widget_t *ti, const char *text) {
    int len = strlen(text);
    if (len >= ti->txt_capacity) len = ti->txt_capacity - 1;
    memcpy(ti->txtbuf, text, len);
    ti->txtbuf[len] = '\0';
    ti->txt_length  = len;
    ti->cursor_pos  = len;
    ti->txt_offset  = 0;
    ti->needs_redraw= true;
}

void tui_textinput_get_text(const tui_widget_t *ti, char *out, int maxlen) {
    int len = ti->txt_length;
    if (len >= maxlen) len = maxlen - 1;
    memcpy(out, ti->txtbuf, len);
    out[len] = '\0';
}