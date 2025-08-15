#include <bareterm_screen.h>
#include <bareterm.h>
#include <widget/bareterm_widget.h>
#include <bareterm_screen_manager.h>
#include <stdio.h>

#include "main_screen.h"
#include "test_screen.h"

// Define the total number of steps in the wizard for clarity.
#define WIZARD_TOTAL_STEPS 3

// --- State for the Wizard ---
static int current_step = 0;

// --- Widgets used across different steps ---
static bareterm_widget_t lbl_title;
static bareterm_widget_t memo_instructions, progress_bar;
static bareterm_widget_t btn_next, btn_back, btn_finish;

// --- Wizard Navigation Callbacks ---

static void on_next_click(void *w) {
    current_step++;
    // Reload the current screen to show the next step
    bareterm_screen_manager_set_active(test_screen_get());
}

static void on_back_click(void *w) {
    current_step--;
    // Reload the current screen to show the previous step
    bareterm_screen_manager_set_active(test_screen_get());
}

static void on_finish_click(void *w) {
    // Reset the step counter for the next time we enter this screen
    current_step = 0;
    // Navigate back to the main screen
    bareterm_screen_manager_set_active(main_screen_get());
}

// --- Screen Lifecycle Functions ---

// on_enter is the "blueprint" for the screen. It runs every time the screen
// is activated, and it uses the `current_step` variable to decide what to draw.
static void on_test_screen_enter(bareterm_screen_t *screen) {
    bareterm_draw_box(1, 1, 80, 30, " Test Procedure ");

    // Initialize the progress bar to show the current step. This is drawn on every step.
    bareterm_progressbar_init(&progress_bar, 5, 26, 40, 1, current_step + 1, WIZARD_TOTAL_STEPS, bareterm_GREEN, bareterm_BLACK, bareterm_STYLE_NONE);

    switch (current_step) {
        case 0: // Step 1: Introduction
            bareterm_label_init(&lbl_title, 5, 3, "Step 1: Introduction", bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
            bareterm_textbox_init(&memo_instructions, 5, 5, 70, 5, "To start tests please follow the instructions below. For questions, contact ismail.tasdemir@analog.com or gokhan.bektas@analog.com.", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);
            
            // Navigation for Step 1
            bareterm_button_init(&btn_next, 66, 25, 12, 3, "[ Next ]", on_next_click);
            break;

        case 1: // Step 2: Visual Inspection
            bareterm_label_init(&lbl_title, 5, 3, "Step 2: Visual Inspection", bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
            bareterm_textbox_init(&memo_instructions, 5, 5, 70, 8, "Check for solder bridges, missing components, and correct shunt placement. Verify jumper settings against the Jumper Settings Table and confirm shunt positions using the 3D board images.", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);

            // Navigation for Step 2
            bareterm_button_init(&btn_back, 52, 25, 12, 3, "[ Back ]", on_back_click);
            bareterm_button_init(&btn_next, 66, 25, 12, 3, "[ Next ]", on_next_click);
            break;

        case 2: // Step 3: Completion
            bareterm_label_init(&lbl_title, 5, 3, "Step 3: Complete", bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
            bareterm_textbox_init(&memo_instructions, 5, 5, 70, 3, "The initial hardware check is complete. Press Finish to return to the main menu.", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);

            // Navigation for Step 3
            bareterm_button_init(&btn_back, 52, 25, 12, 3, "[ Back ]", on_back_click);
            bareterm_button_init(&btn_finish, 66, 25, 12, 3, "[ Finish ]", on_finish_click);
            break;

        default:
            // Invalid step, reset to the beginning to be safe
            current_step = 0;
            bareterm_screen_manager_set_active(test_screen_get());
            break;
    }
}

// --- Screen Definition ---
static bareterm_screen_t test_screen_def = {
    .on_enter = on_test_screen_enter,
    .on_leave = NULL, // The state is reset in on_finish_click now
    .user_data = NULL,
};

bareterm_screen_t* test_screen_get(void) {
    return &test_screen_def;
}
