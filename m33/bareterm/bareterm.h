// ============================
// File: tui.h
// ============================
#ifndef bareterm_H
#define bareterm_H

#include <stddef.h>
#include <stdarg.h>


#include "bareterm_input.h"
#include "bareterm_uart_input.h"
#include "bareterm_backend.h"


#ifdef __cplusplus
extern "C" {
#endif

void bareterm_init(void);
void bareterm_clear_screen(void);
void bareterm_move_cursor(int col, int row);
void bareterm_puts(const char *str);
void bareterm_printf(const char *fmt, ...);

// Color handling
typedef enum {
    bareterm_BLACK = 0,
    bareterm_RED,
    bareterm_GREEN,
    bareterm_YELLOW,
    bareterm_BLUE,
    bareterm_MAGENTA,
    bareterm_CYAN,
    bareterm_WHITE
} bareterm_color_t;

typedef enum {
    bareterm_STYLE_NONE      = 0,
    bareterm_STYLE_BOLD      = 1 << 0,
    bareterm_STYLE_UNDERLINE = 1 << 1
} bareterm_style_t;

void bareterm_set_color(bareterm_color_t fg, bareterm_color_t bg, bareterm_style_t style);
void bareterm_reset_color(void);

// Box drawing
void bareterm_draw_box(int x, int y, int width, int height, const char *title);



#ifdef __cplusplus
}
#endif

#endif // bareterm_H