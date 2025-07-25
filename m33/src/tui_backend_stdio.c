// tui_backend_stdio.c
#include "tui_backend.h"
#include <stdio.h>
#include <string.h>

void tui_backend_write(const char *buf, size_t len) {
    fwrite(buf, 1, len, stdout);  // or: write to UART buffer in future
}