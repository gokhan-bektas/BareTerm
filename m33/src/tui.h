// ============================
// File: tui.h
// ============================
#ifndef TUI_H
#define TUI_H

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void tui_init(void);
void tui_clear_screen(void);
void tui_move_cursor(int col, int row);
void tui_puts(const char *str);
void tui_printf(const char *fmt, ...);

// Color handling
typedef enum {
    TUI_BLACK = 0,
    TUI_RED,
    TUI_GREEN,
    TUI_YELLOW,
    TUI_BLUE,
    TUI_MAGENTA,
    TUI_CYAN,
    TUI_WHITE
} tui_color_t;

typedef enum {
    TUI_STYLE_NONE      = 0,
    TUI_STYLE_BOLD      = 1 << 0,
    TUI_STYLE_UNDERLINE = 1 << 1
} tui_style_t;

void tui_set_color(tui_color_t fg, tui_color_t bg, tui_style_t style);
void tui_reset_color(void);

// Box drawing
void tui_draw_box(int x, int y, int width, int height, const char *title);

#ifdef __cplusplus
}
#endif

#endif // TUI_H