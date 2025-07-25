// tui_event_loop.c
#include "tui_event.h"
#include "tui.h"

void tui_event_ltui_event_loopoop(void) {
    tui_event_t evt;

    while (1) {
        if (k_msgq_get(&tui_event_q, &evt, K_FOREVER) == 0) {
            switch (evt.type) {
                case TUI_EVENT_KEY:
                    // Temporary: just echo the key
                    tui_move_cursor(1, 20);
                    tui_printf("Key: '%c' (0x%02X)", evt.key.ch, evt.key.ch);
                    break;

                case TUI_EVENT_MOUSE:
                    // Future
                    break;
            }
        }
    }
}
