// ============================
// File: tui_input.c
// ============================
#include "tui_input.h"

static tui_event_t pending_event;
static int event_ready = 0;

// Escape sequence parser state
typedef enum {
    ESC_IDLE,
    ESC_GOT_ESC,
    ESC_GOT_BRACKET,
    ESC_MOUSE_WAIT_1,
    ESC_MOUSE_WAIT_2,
    ESC_MOUSE_WAIT_3
} esc_state_t;

static esc_state_t esc_state = ESC_IDLE;

static char esc_buf[3];
static int last_button = 0; // remember last pressed button for release events


void tui_input_feed(char byte) {
    if (event_ready) {
        return;
    }

    switch (esc_state) {
        case ESC_IDLE:
            if (byte == 0x1B) {
                esc_state = ESC_GOT_ESC;
            } else {
                pending_event.type     = TUI_EVT_KEY;
                pending_event.key.ch   = byte;
                pending_event.key.code = 0;
                event_ready            = 1;
            }
            break;

        case ESC_GOT_ESC:
            if (byte == '[') {
                esc_state = ESC_GOT_BRACKET;
            } else {
                // stray ESC
                pending_event.type     = TUI_EVT_KEY;
                pending_event.key.ch   = 0x1B;
                pending_event.key.code = 0;
                event_ready            = 1;
                esc_state              = ESC_IDLE;
            }
            break;

        case ESC_GOT_BRACKET:
            if (byte == 'M') {
                esc_state = ESC_MOUSE_WAIT_1;
            } else {
                // arrow keys
                pending_event.type     = TUI_EVT_ARROW;
                pending_event.key.ch   = 0;
                switch (byte) {
                    case 'A': pending_event.key.code = TUI_KEY_UP;    break;
                    case 'B': pending_event.key.code = TUI_KEY_DOWN;  break;
                    case 'C': pending_event.key.code = TUI_KEY_RIGHT; break;
                    case 'D': pending_event.key.code = TUI_KEY_LEFT;  break;
                    default:  pending_event.key.code = 0;             break;
                }
                event_ready = 1;
                esc_state   = ESC_IDLE;
            }
            break;

        case ESC_MOUSE_WAIT_1:
            // first mouse byte = button state
            esc_buf[0] = byte;
            esc_state  = ESC_MOUSE_WAIT_2;
            break;

        case ESC_MOUSE_WAIT_2:
            // second mouse byte = X + 32
            esc_buf[1] = byte;
            esc_state  = ESC_MOUSE_WAIT_3;
            break;

        case ESC_MOUSE_WAIT_3:
            // third mouse byte = Y + 32
            esc_buf[2] = byte;
            // emit mouse event
            pending_event.type          = TUI_EVT_MOUSE;
            pending_event.mouse.button  = esc_buf[0] & 0x03;
            pending_event.mouse.pressed = (esc_buf[0] & 0x03) != 3;
            pending_event.mouse.x       = esc_buf[1] - 32;
            pending_event.mouse.y       = esc_buf[2] - 32;
            event_ready                 = 1;
            esc_state                   = ESC_IDLE;
            break;
    }
}

int tui_poll_event(tui_event_t *event) {
    if (event_ready) {
        *event = pending_event;
        event_ready = 0;
        return 1;
    }
    return 0;
}