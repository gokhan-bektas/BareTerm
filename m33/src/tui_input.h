// ============================
// File: tui_input.h
// ============================
#ifndef TUI_INPUT_H
#define TUI_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TUI_EVT_KEY,
    TUI_EVT_ARROW,
    TUI_EVT_MOUSE
} tui_event_type_t;

// Special key codes
#define TUI_KEY_UP     1001
#define TUI_KEY_DOWN   1002
#define TUI_KEY_RIGHT  1003
#define TUI_KEY_LEFT   1004
#define TUI_KEY_ENTER  13

typedef struct {
    tui_event_type_t type;
    union {
        struct {
            char ch;
            int code;
        } key;
        struct {
            int x;
            int y;
            int button;
            int pressed; // 1 = press, 0 = release
        } mouse;
    };
} tui_event_t;

void tui_input_feed(char byte);
int tui_poll_event(tui_event_t *event);

#ifdef __cplusplus
}
#endif

#endif // TUI_INPUT_H