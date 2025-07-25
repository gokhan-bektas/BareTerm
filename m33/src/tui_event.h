// tui_event.h
#ifndef TUI_EVENT_H
#define TUI_EVENT_H

typedef enum {
    TUI_EVENT_KEY,
    TUI_EVENT_MOUSE
} tui_event_type_t;

typedef struct {
    tui_event_type_t type;
    union {
        struct {
            char ch;        // for ASCII keys
            int code;       // for special keys (e.g. arrows)
        } key;

        struct {
            int x, y;
            int button;
            int pressed;    // 1 = press, 0 = release
        } mouse;
    };
} tui_event_t;

#endif
