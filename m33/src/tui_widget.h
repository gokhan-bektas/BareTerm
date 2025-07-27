// ============================
// File: tui_widget.h
// ============================
#ifndef TUI_WIDGET_H
#define TUI_WIDGET_H

#include "tui.h"
#include "tui_input.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
typedef struct tui_widget tui_widget_t;

// Function pointer types for widget operations
typedef void (*tui_widget_draw_fn)(tui_widget_t *widget);
typedef void (*tui_widget_event_fn)(tui_widget_t *widget, const tui_event_t *evt);

// Base widget structure
typedef struct tui_widget {
    int x, y;                  // position
    int width, height;         // size
    bool focused;              // focus state
    bool visible;              // visibility flag
    bool needs_redraw;         // dirty flag
    tui_widget_draw_fn draw;   // draw method
    tui_widget_event_fn handle_event; // event handler
    struct tui_widget *parent;     // parent container
    struct tui_widget *next_sibling; // next in focus order
    void *user_data;           // widget-specific data
    const char *label;         // label text for buttons/labels
    void (*click_cb)(void *);  // callback for button click           // widget-specific data
} tui_widget_t;

// Register a widget for drawing and event dispatch
void tui_widget_register(tui_widget_t *widget);

// Draw all registered widgets
void tui_draw_all_widgets(void);

// Dispatch an event to the focused widget
void tui_dispatch_event(const tui_event_t *evt);

// Button widget initializer
void tui_button_init(tui_widget_t *btn,
                     int x, int y, int w, int h,
                     const char *label,
                     void (*on_click)(void *));

#ifdef __cplusplus
}
#endif

#endif // TUI_WIDGET_H