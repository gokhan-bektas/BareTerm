// tui_backend.h
#ifndef TUI_BACKEND_H
#define TUI_BACKEND_H

#include <stddef.h>


// You define this function in the platform layer
void tui_backend_write(const char *buf, size_t len);

// Convenience macro for string literals
#define tui_backend_puts(str) tui_backend_write(str, sizeof(str) - 1)

#endif // TUI_BACKEND_H