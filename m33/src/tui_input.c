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
    ESC_MOUSE_WAIT_1,  // saw the 'M'
    ESC_MOUSE_WAIT_2,  // saw cb
    ESC_MOUSE_WAIT_3   // saw x, now need y
} esc_state_t;

static char esc_buf[3];
static int esc_state = 0;

void tui_input_feed(char byte) {

    //tf("tui_input_feed: byte=0x%02X\n", (unsigned char)byte);
    if (event_ready) {
        // still holding a previous event until it's consumed
        return;
    }

    switch (esc_state) {
        case ESC_IDLE:
            if ((unsigned char)byte == 0x1B) {
                // start of an ESC sequence
                esc_state = ESC_GOT_ESC;
            } else {
                // regular key
                pending_event.type        = TUI_EVT_KEY;
                pending_event.key.ch      = byte;
                pending_event.key.code    = 0;
                event_ready               = 1;
            }
            break;

        case ESC_GOT_ESC:
            if (byte == '[') {
                // CSI
                esc_state = ESC_GOT_BRACKET;
                //printf("GOT [\n");
            } else {
                // stray ESC
                pending_event.type        = TUI_EVT_KEY;
                pending_event.key.ch      = 0x1B;
                pending_event.key.code    = 0;
                event_ready               = 1;
                esc_state                 = ESC_IDLE;
            }
            break;

        case ESC_GOT_BRACKET:
            if (byte == 'M') {
                // mouse sequence Starter
                esc_state = ESC_MOUSE_WAIT_1;
            } else {
                // arrow or other CSI<byte>
                pending_event.type     = TUI_EVT_KEY;
                pending_event.key.ch   = 0;
                //printf("GOT CSI: %c\n", byte);
                switch (byte) {
                    case 'A': pending_event.key.code = TUI_KEY_UP;    pending_event.type = TUI_EVT_ARROW; break;
                    case 'B': pending_event.key.code = TUI_KEY_DOWN;  pending_event.type = TUI_EVT_ARROW; break;
                    case 'C': pending_event.key.code = TUI_KEY_RIGHT; pending_event.type = TUI_EVT_ARROW; break;
                    case 'D': pending_event.key.code = TUI_KEY_LEFT;  pending_event.type = TUI_EVT_ARROW; break;
                    default:  pending_event.key.code = 0;             break;
                }
                event_ready = 1;
                esc_state   = ESC_IDLE;
            }
            break;

        case ESC_MOUSE_WAIT_1:
            // cb byte (button + modifier)
            esc_buf[0] = byte;
            esc_state  = ESC_MOUSE_WAIT_2;
            break;

        case ESC_MOUSE_WAIT_2:
            // X coordinate (column + 32)
            esc_buf[1] = byte;
            esc_state  = ESC_MOUSE_WAIT_3;
            break;

        case ESC_MOUSE_WAIT_3:
            // Y coordinate (row + 32)
            esc_buf[2] = byte;
            // now emit mouse event
            pending_event.type              = TUI_EVT_MOUSE;
            pending_event.mouse.button      = esc_buf[0] & 0x03;
            pending_event.mouse.pressed     = !(esc_buf[0] & 0x20);
            pending_event.mouse.x           = esc_buf[1] - 32;
            pending_event.mouse.y           = esc_buf[2] - 32;
            event_ready                     = 1;
            esc_state                       = ESC_IDLE;
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
