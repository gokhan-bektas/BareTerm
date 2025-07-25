// tui_input.c
#include "tui_event.h"
#include <zephyr/kernel.h>

K_MSGQ_DEFINE(tui_event_q, sizeof(tui_event_t), 8, 4);  // global input queue

void tui_input_feed_byte(char c) {
    tui_event_t evt = { .type = TUI_EVENT_KEY, .key = { .ch = c, .code = 0 } };

    // TODO: Parse ESC sequences like \x1B[A, \x1B[<x;y;zM etc.
    if (c == '\x1B') {
        // begin escape sequence parsing state machine (future step)
    }

    k_msgq_put(&tui_event_q, &evt, K_NO_WAIT);
}