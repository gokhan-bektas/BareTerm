// ============================
// File: tui.c
// ============================
#include "bareterm.h"
#include <stdio.h>
#include <string.h>
#include <zephyr/drivers/uart.h>


// Backend hook (weak by default)
__attribute__((weak)) void bareterm_backend_write(const char *buf, size_t len) {
    fwrite(buf, 1, len, stdout);
}

void bareterm_init(void) {
    // Placeholder for backend-specific init (e.g., UART)
    // Enable mouse tracking (button press/release, no motion tracking)
    bareterm_backend_write("\x1B[?1000h", 8);
    
}

void bareterm_clear_screen(void) {
    bareterm_backend_write("\x1B[2J\x1B[H", 7);
}

void bareterm_move_cursor(int col, int row) {
    char buf[16];
    int len = snprintf(buf, sizeof(buf), "\x1B[%d;%dH", row, col);
    bareterm_backend_write(buf, len);
}

void bareterm_puts(const char *str) {
    if (str) {
        bareterm_backend_write(str, strlen(str));
    }
}

void bareterm_printf(const char *fmt, ...) {
    char buf[128];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (len > 0) {
        if ((size_t)len > sizeof(buf)) len = sizeof(buf);
        bareterm_backend_write(buf, len);
    }
}

void bareterm_set_color(bareterm_color_t fg, bareterm_color_t bg, bareterm_style_t style) {
    char buf[64];
    int offset = 0;

    if (style & bareterm_STYLE_BOLD) {
        offset += snprintf(buf + offset, sizeof(buf) - offset, "\x1B[1m");
    }
    if (style & bareterm_STYLE_UNDERLINE) {
        offset += snprintf(buf + offset, sizeof(buf) - offset, "\x1B[4m");
    }

    offset += snprintf(buf + offset, sizeof(buf) - offset, "\x1B[%d;%dm", 30 + fg, 40 + bg);
    bareterm_backend_write(buf, offset);
}

void bareterm_reset_color(void) {
    bareterm_backend_write("\x1B[0m", 4);
}

void bareterm_draw_box(int x, int y, int width, int height, const char *title) {
    const char *TL = "\xE2\x94\x8C"; // ┌
    const char *TR = "\xE2\x94\x90"; // ┐
    const char *BL = "\xE2\x94\x94"; // └
    const char *BR = "\xE2\x94\x98"; // ┘
    const char *H  = "\xE2\x94\x80"; // ─
    const char *V  = "\xE2\x94\x82"; // │

    // Top border
    bareterm_move_cursor(x, y);
    bareterm_puts(TL);
    for (int i = 0; i < width - 2; ++i) bareterm_puts(H);
    bareterm_puts(TR);

    // Title
    if (title && width > 4) {
        bareterm_move_cursor(x + 2, y);
        bareterm_puts(title);
    }

    // Sides
    for (int i = 1; i < height - 1; ++i) {
        bareterm_move_cursor(x, y + i);
        bareterm_puts(V);
        bareterm_move_cursor(x + width - 1, y + i);
        bareterm_puts(V);
    }

    // Bottom border
    bareterm_move_cursor(x, y + height - 1);
    bareterm_puts(BL);
    for (int i = 0; i < width - 2; ++i) bareterm_puts(H);
    bareterm_puts(BR);
}