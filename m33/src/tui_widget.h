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
    int x, y;                         // position
    int width, height;                // size
    bool focused;                     // focus state
    bool visible;                     // visibility flag
    bool needs_redraw;                // dirty flag
    tui_widget_draw_fn draw;          // draw method
    tui_widget_event_fn handle_event; // event handler
    struct tui_widget *parent;        // parent container
    struct tui_widget *next_sibling;  // next in focus order
    void *user_data;                  // widget-specific data
    const char *label;                // label text for buttons/labels
    void (*click_cb)(void *);         // callback for button click           // widget-specific data
    int value;                        // value for progress bars
    int max;                          // max value for progress bars
    tui_color_t fg, bg;
    tui_style_t style;
} tui_widget_t;

// Register a widget for drawing and event dispatch
void tui_widget_register(tui_widget_t *widget);

// Draw all registered widgets
void tui_draw_all_widgets(void);

// Dispatch an event to the focused widget
void tui_dispatch_event(const tui_event_t *evt);

void tui_label_init(tui_widget_t *lbl,
                    int x, int y,
                    const char *text,
                    tui_color_t fg, tui_color_t bg,
                    tui_style_t style);
// Update label text at runtime
void tui_label_set_text(tui_widget_t *lbl, const char *text);

// Update label color and style at runtime
void tui_label_set_color(tui_widget_t *lbl, tui_color_t fg, tui_color_t bg, tui_style_t style);


static void textbox_draw(tui_widget_t *w);
static void textbox_event(tui_widget_t *w, const tui_event_t *evt);
void tui_textbox_init(tui_widget_t *tb,
                      int x, int y, int w, int h,
                      const char *text,
                      tui_color_t fg, tui_color_t bg, tui_style_t style);
void tui_textbox_set_text(tui_widget_t *tb, const char *text);
void tui_textbox_set_color(tui_widget_t *tb, tui_color_t fg, tui_color_t bg, tui_style_t style);
// ============================
// ProgressBar widget initializer and update functions
// ============================
// Initialize a horizontal progress bar widget
void tui_progressbar_init(tui_widget_t *pb,
                          int x, int y, int w, int h,
                          int value, int max,
                          tui_color_t fg, tui_color_t bg, tui_style_t style);

// Update progress bar value at runtime
void tui_progressbar_set_value(tui_widget_t *pb, int value);                      

#ifdef __cplusplus
}
#endif

#endif // TUI_WIDGET_H