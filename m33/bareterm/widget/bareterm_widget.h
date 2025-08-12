// ============================
// File: bareterm_widget.h
// ============================
#ifndef bareterm_WIDGET_H
#define bareterm_WIDGET_H

#include "../bareterm.h"
#include "bareterm_input.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
typedef struct bareterm_widget bareterm_widget_t;

// Function pointer types for widget operations
typedef void (*bareterm_widget_draw_fn)(bareterm_widget_t *widget);
typedef void (*bareterm_widget_event_fn)(bareterm_widget_t *widget, const bareterm_event_t *evt);

// Base widget structure
typedef struct bareterm_widget {
    int x, y;                         // position
    int width, height;                // size
    bool focused;                     // focus state
    bool visible;                     // visibility flag
    bool needs_redraw;                // dirty flag
    bareterm_widget_draw_fn draw;          // draw method
    bareterm_widget_event_fn handle_event; // event handler
    struct bareterm_widget *parent;        // parent container
    struct bareterm_widget *next_sibling;  // next in focus order
    void *user_data;                  // widget-specific data
    const char *label;                // label text for buttons/labels
    void (*click_cb)(void *);         // callback for button click           // widget-specific data
    int value;                        // value for progress bars
    int max;                          // max value for progress bars
    bareterm_color_t fg, bg;
    bareterm_style_t style;
    int checked;                     // checkbox state (0 = unchecked, 1 = checked)
    void (*toggle_cb)(bareterm_widget_t *cb, unsigned char new_state); // callback for checkbox toggle
    const char **rg_labels;                    // array of option strings
    int           rg_count;                    // number of options
    int           rg_selected;                 // index of the currently selected option
    void        (*rg_change_cb)(bareterm_widget_t*, int); // callback(new_index)
    char    *txtbuf;        // pointer to your text buffer
    int      txt_capacity;  // total buffer capacity (including NUL)
    int      txt_length;    // current string length
    int      cursor_pos;    // insertion cursor position [0..txt_length]
    int      txt_offset;    // first visible character index
    void   (*submit_cb)(bareterm_widget_t *ti); // called on Enter
    
    // Listbox-specific data
    const char **lb_items;
    int          lb_count;
    int          lb_selected;
    int          lb_top;
    void       (*lb_select_cb)(bareterm_widget_t*, int);
    
} bareterm_widget_t;

// Register a widget for drawing and event dispatch
void bareterm_widget_register(bareterm_widget_t *widget);

// Draw all registered widgets
void bareterm_draw_all_widgets(void);

// Dispatch an event to the focused widget
void bareterm_dispatch_event(const bareterm_event_t *evt);

void bareterm_label_init(bareterm_widget_t *lbl,
                    int x, int y,
                    const char *text,
                    bareterm_color_t fg, bareterm_color_t bg,
                    bareterm_style_t style);
// Update label text at runtime
void bareterm_label_set_text(bareterm_widget_t *lbl, const char *text);

// Update label color and style at runtime
void bareterm_label_set_color(bareterm_widget_t *lbl, bareterm_color_t fg, bareterm_color_t bg, bareterm_style_t style);


void bareterm_textbox_init(bareterm_widget_t *tb,
                      int x, int y, int w, int h,
                      const char *text,
                      bareterm_color_t fg, bareterm_color_t bg, bareterm_style_t style);
void bareterm_textbox_set_text(bareterm_widget_t *tb, const char *text);
void bareterm_textbox_set_color(bareterm_widget_t *tb, bareterm_color_t fg, bareterm_color_t bg, bareterm_style_t style);
// ============================
// ProgressBar widget initializer and update functions
// ============================
// Initialize a horizontal progress bar widget
void bareterm_progressbar_init(bareterm_widget_t *pb,
                          int x, int y, int w, int h,
                          int value, int max,
                          bareterm_color_t fg, bareterm_color_t bg, bareterm_style_t style);

// Update progress bar value at runtime
void bareterm_progressbar_set_value(bareterm_widget_t *pb, int value);                      



// Checkbox widget initializer
// cb:       pointer to your widget struct
// x,y:      upper‑left position
// label:    text to display to the right of the box
// initial:  starting checked state
// on_toggle: callback(widget, new_state) when user toggles
void bareterm_checkbox_init(bareterm_widget_t *cb,
                       int x, int y,
                       const char *label,
                       unsigned char initial,
                       void (*on_toggle)(bareterm_widget_t *cb, unsigned char new_state));

// Programmatically set/get its state
void bareterm_checkbox_set_checked(bareterm_widget_t *cb, unsigned char checked);
unsigned char bareterm_checkbox_is_checked(const bareterm_widget_t *cb);


void bareterm_modalpopup_show(const char *title, const char *msg);

// Public API (below your other inits):
// Initialize a radio‐button group
// - labels: array of `count` C‐strings (must stay valid)
// - initial: starting selection [0..count-1]
// - on_change: called when selection changes
void bareterm_radiogroup_init(bareterm_widget_t *rg,
                         int x, int y,
                         const char **labels,
                         int count,
                         int initial,
                         void (*on_change)(bareterm_widget_t *rg, int selected));

// Programmatically set/get
void bareterm_radiogroup_set_selected(bareterm_widget_t *rg, int selected);
int  bareterm_radiogroup_get_selected(const bareterm_widget_t *rg);

/**
 * Initialize a single‑line text‑input widget.
 *
 * ti:        pointer to a bareterm_widget_t instance
 * x,y:       top‑left position
 * w:         width in characters
 * buffer:    pointer to a char array of size >= capacity
 * capacity:  total buffer size (including space for trailing NUL)
 * on_submit: callback invoked (with the widget) when Enter is pressed
 */
void bareterm_textinput_init(bareterm_widget_t *ti,
                        int x, int y, int w,
                        char *buffer, int capacity,
                        void (*on_submit)(bareterm_widget_t *ti));

/** Replace the entire contents of the input. */
void bareterm_textinput_set_text(bareterm_widget_t *ti, const char *text);

/** Copy out up to maxlen‑1 characters + NUL. */
void bareterm_textinput_get_text(const bareterm_widget_t *ti,
                            char *out, int maxlen);

// Listbox widget initializer
void bareterm_listbox_init(bareterm_widget_t *lb,
                      int x, int y, int w, int h,
                      const char **items, int count,
                      void (*on_select)(bareterm_widget_t *lb, int selected));

#ifdef __cplusplus
}
#endif

#endif // bareterm_WIDGET_H