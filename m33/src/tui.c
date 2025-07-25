// tui.c
#include "tui.h"
#include "tui_backend.h"
#include "tui_color.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#define TUI_PRINTF_BUFFER_SIZE 128

void tui_clear_screen(void) {
    tui_backend_puts("\x1B[2J\x1B[H");
}


void tui_set_color(tui_color_t fg, tui_color_t bg, tui_style_t style) {
    char buf[64];
    int offset = 0;

    // Add style codes first
    if (style & TUI_STYLE_BOLD) {
        offset += snprintf(buf + offset, sizeof(buf) - offset, "\x1B[1m");
    }
    if (style & TUI_STYLE_UNDERLINE) {
        offset += snprintf(buf + offset, sizeof(buf) - offset, "\x1B[4m");
    }

    // Add color codes (30–37, 40–47)
    offset += snprintf(buf + offset, sizeof(buf) - offset, "\x1B[%d;%dm", 30 + fg, 40 + bg);

    tui_backend_write(buf, offset);
}

void tui_set_color_rgb(uint8_t fr, uint8_t fg, uint8_t fb,
                       uint8_t br, uint8_t bg, uint8_t bb,
                       tui_style_t style) {
    char buf[128];
    int offset = 0;

    if (style & TUI_STYLE_BOLD)       offset += snprintf(buf + offset, sizeof(buf) - offset, "\x1B[1m");
    if (style & TUI_STYLE_UNDERLINE)  offset += snprintf(buf + offset, sizeof(buf) - offset, "\x1B[4m");

    // 24-bit truecolor ANSI sequences
    offset += snprintf(buf + offset, sizeof(buf) - offset,
                       "\x1B[38;2;%d;%d;%dm\x1B[48;2;%d;%d;%dm",
                       fr, fg, fb, br, bg, bb);

    tui_backend_write(buf, offset);
}

void tui_reset_color(void) {
    tui_backend_puts("\x1B[0m");
}

void tui_puts(const char *str) {
    if (str) {
        tui_backend_write(str, strlen(str));
    }
}

void tui_printf(const char *fmt, ...) {
    static char buf[TUI_PRINTF_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (len > 0) {
        // Clip to buffer size just in case
        if ((size_t)len >= sizeof(buf)) {
            len = sizeof(buf) - 1;
            buf[len] = '\0';
        }
        tui_backend_write(buf, len);
    }
}

void tui_move_cursor(int col, int row) {
    char buf[16];
    // ANSI cursor move: ESC[row;colH]
    int len = snprintf(buf, sizeof(buf), "\x1B[%d;%dH", row, col);
    tui_backend_write(buf, len);
}


void tui_draw_box(int x, int y, int width, int height, const char *title) {
    // Unicode box-drawing characters
    const char *TL = "┌";
    const char *TR = "┐";
    const char *BL = "└";
    const char *BR = "┘";
    const char *H  = "─";
    const char *V  = "│";

    char line[128];

    // Top border
    tui_move_cursor(x, y);
    tui_puts(TL);
    for (int i = 0; i < width - 2; i++) tui_puts(H);
    tui_puts(TR);

    // Title (if provided)
    if (title && strlen(title) > 0 && width > 4) {
        tui_move_cursor(x + 2, y);  // leave room from left
        tui_puts(title);
    }

    // Vertical sides
    for (int i = 1; i < height - 1; i++) {
        tui_move_cursor(x, y + i);
        tui_puts(V);
        tui_move_cursor(x + width - 1, y + i);
        tui_puts(V);
    }

    // Bottom border
    tui_move_cursor(x, y + height - 1);
    tui_puts(BL);
    for (int i = 0; i < width - 2; i++) tui_puts(H);
    tui_puts(BR);
}