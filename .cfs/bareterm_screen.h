#ifndef BARETERM_SCREEN_H
#define BARETERM_SCREEN_H

#include "widget/bareterm_widget.h"

typedef struct bareterm_screen {
    // Called when the screen becomes active.
    // Should create and register all widgets for this screen.
    void (*on_enter)(struct bareterm_screen *screen);

    // Called when the screen is about to be replaced.
    // Can be used for cleanup.
    void (*on_leave)(struct bareterm_screen *screen);

    // Optional user data
    void *user_data;
} bareterm_screen_t;

#endif // BARETERM_SCREEN_H