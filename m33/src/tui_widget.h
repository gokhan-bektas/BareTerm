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
    int checked;                     // checkbox state (0 = unchecked, 1 = checked)
    void (*toggle_cb)(tui_widget_t *cb, unsigned char new_state); // callback for checkbox toggle
    const char **rg_labels;                    // array of option strings
    int           rg_count;                    // number of options
    int           rg_selected;                 // index of the currently selected option
    void        (*rg_change_cb)(tui_widget_t*, int); // callback(new_index)
    char    *txtbuf;        // pointer to your text buffer
    int      txt_capacity;  // total buffer capacity (including NUL)
    int      txt_length;    // current string length
    int      cursor_pos;    // insertion cursor position [0..txt_length]
    int      txt_offset;    // first visible character index
    void   (*submit_cb)(tui_widget_t *ti); // called on Enter

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



// Checkbox widget initializer
// cb:       pointer to your widget struct
// x,y:      upper‑left position
// label:    text to display to the right of the box
// initial:  starting checked state
// on_toggle: callback(widget, new_state) when user toggles
void tui_checkbox_init(tui_widget_t *cb,
                       int x, int y,
                       const char *label,
                       unsigned char initial,
                       void (*on_toggle)(tui_widget_t *cb, unsigned char new_state));

// Programmatically set/get its state
void tui_checkbox_set_checked(tui_widget_t *cb, unsigned char checked);
unsigned char tui_checkbox_is_checked(const tui_widget_t *cb);


void tui_modalpopup_show(const char *title, const char *msg);

// Public API (below your other inits):
// Initialize a radio‐button group
// - labels: array of `count` C‐strings (must stay valid)
// - initial: starting selection [0..count-1]
// - on_change: called when selection changes
void tui_radiogroup_init(tui_widget_t *rg,
                         int x, int y,
                         const char **labels,
                         int count,
                         int initial,
                         void (*on_change)(tui_widget_t *rg, int selected));

// Programmatically set/get
void tui_radiogroup_set_selected(tui_widget_t *rg, int selected);
int  tui_radiogroup_get_selected(const tui_widget_t *rg);

/**
 * Initialize a single‑line text‑input widget.
 *
 * ti:        pointer to a tui_widget_t instance
 * x,y:       top‑left position
 * w:         width in characters
 * buffer:    pointer to a char array of size >= capacity
 * capacity:  total buffer size (including space for trailing NUL)
 * on_submit: callback invoked (with the widget) when Enter is pressed
 */
void tui_textinput_init(tui_widget_t *ti,
                        int x, int y, int w,
                        char *buffer, int capacity,
                        void (*on_submit)(tui_widget_t *ti));

/** Replace the entire contents of the input. */
void tui_textinput_set_text(tui_widget_t *ti, const char *text);

/** Copy out up to maxlen‑1 characters + NUL. */
void tui_textinput_get_text(const tui_widget_t *ti,
                            char *out, int maxlen);

#ifdef __cplusplus
}
#endif

#endif // TUI_WIDGET_H