// tui.h

#ifndef TUI_H
#define TUI_H

#include "tui_color.h"
#include <stdint.h>

void tui_clear_screen(void);
void tui_puts(const char *str);
void tui_printf(const char *fmt, ...); 
void tui_move_cursor(int col, int row);  // column = x, row = y
void tui_set_color_rgb(uint8_t fr, uint8_t fg, uint8_t fb,
                       uint8_t br, uint8_t bg, uint8_t bb,
                       tui_style_t style);
void tui_set_color(tui_color_t fg, tui_color_t bg, tui_style_t style);       
void tui_draw_box(int x, int y, int width, int height, const char *title);                
#endif // TUI_H