#include "bareterm_screen_manager.h"
#include "widget/bareterm_widget.h"
#include "bareterm.h"

static bareterm_screen_t *active_screen = NULL;

void bareterm_screen_manager_init(void) {
    active_screen = NULL;
}

void bareterm_screen_manager_set_active(bareterm_screen_t *screen) {
    if (active_screen) {
        if (active_screen->on_leave) {
            active_screen->on_leave(active_screen);
        }
    }

    // Clear the global widget registry
    bareterm_widget_unregister_all();
    
    // Clear the physical screen
    bareterm_clear_screen();

    active_screen = screen;

    if (active_screen) {
        if (active_screen->on_enter) {
            active_screen->on_enter(active_screen);
        }
    }
    
    // The new screen's on_enter has registered new widgets.
    // Mark them all for drawing.
    bareterm_draw_all_widgets();
}

bareterm_screen_t* bareterm_screen_manager_get_active(void) {
    return active_screen;
}

