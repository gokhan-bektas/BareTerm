#ifndef TUI_COLOR_H
#define TUI_COLOR_H

#include <stdint.h>


typedef enum {
    TUI_BLACK   = 0,
    TUI_RED     = 1,
    TUI_GREEN   = 2,
    TUI_YELLOW  = 3,
    TUI_BLUE    = 4,
    TUI_MAGENTA = 5,
    TUI_CYAN    = 6,
    TUI_WHITE   = 7
} tui_color_t;

typedef enum {
    TUI_STYLE_NONE      = 0,
    TUI_STYLE_BOLD      = 1 << 0,
    TUI_STYLE_UNDERLINE = 1 << 1
} tui_style_t;

void tui_set_color(tui_color_t fg, tui_color_t bg, tui_style_t style);
void tui_reset_color(void);

// Optional: for full RGB support
void tui_set_color_rgb(uint8_t fr, uint8_t fg, uint8_t fb,
                       uint8_t br, uint8_t bg, uint8_t bb,
                       tui_style_t style);

#endif // TUI_COLOR_H