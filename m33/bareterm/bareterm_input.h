// ============================
// File: bareterm_input.h
// ============================
#ifndef bareterm_INPUT_H
#define bareterm_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    bareterm_EVT_KEY,
    bareterm_EVT_ARROW,
    bareterm_EVT_MOUSE
} bareterm_event_type_t;

// Special key codes
#define bareterm_KEY_UP     1001
#define bareterm_KEY_DOWN   1002
#define bareterm_KEY_RIGHT  1003
#define bareterm_KEY_LEFT   1004
#define bareterm_KEY_ENTER  13

typedef struct {
    bareterm_event_type_t type;
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
} bareterm_event_t;

void bareterm_input_feed(char byte);
int bareterm_poll_event(bareterm_event_t *event);

#ifdef __cplusplus
}
#endif

#endif // bareterm_INPUT_H