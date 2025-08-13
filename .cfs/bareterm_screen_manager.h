#ifndef BARETERM_SCREEN_MANAGER_H
#define BARETERM_SCREEN_MANAGER_H

#include "bareterm_screen.h"

void bareterm_screen_manager_init(void);

// Sets the new active screen, triggering on_leave and on_enter
void bareterm_screen_manager_set_active(bareterm_screen_t *screen);

// Gets the currently active screen
bareterm_screen_t* bareterm_screen_manager_get_active(void);

#endif // BARETERM_SCREEN_MANAGER_H