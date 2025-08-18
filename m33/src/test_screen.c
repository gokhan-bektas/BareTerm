#include <bareterm_screen.h>
#include <bareterm.h>
#include <widget/bareterm_widget.h>
#include <bareterm_screen_manager.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include "main_screen.h"
#include "test_screen.h"

// Use an enum for wizard steps to improve readability and maintainability.
typedef enum {
    WIZARD_STEP_INTRO,
    WIZARD_STEP_INSPECTION,
    WIZARD_STEP_VOLTAGES,
    WIZARD_STEP_LED_TEST,
    WIZARD_STEP_GPIO_TEST_PREPARE,
    WIZARD_STEP_GPIO_TEST_PERFORM,
    WIZARD_STEP_COMPLETE,
    WIZARD_STEP_COUNT // This special member gives us the total number of steps.
} wizard_step_t;

typedef enum {
    TESTERROR_LED,
    TESTERROR_GPIO
} test_error_t;

// --- State for the Wizard ---
// The current step is now of the enum type, initialized to the first step.
static wizard_step_t current_step = WIZARD_STEP_INTRO;

// State for the voltage check step. This must be static to persist
// when the user navigates back and forth.
static bool tp3_vbus_checked = false;
static bool tp5_vin_checked = false;
static bool tp13_vdd18_checked = false;
static bool tp8_vreg1_checked = false;
static bool tp10_vcore_checked = false;
static bool cb_ledd0_checked = false;
static bool cb_ledd4_checked = false;

// --- Widgets used across different steps ---
static bareterm_widget_t lbl_title;
static bareterm_widget_t memo_instructions, progress_bar;
static bareterm_widget_t btn_next, btn_back, btn_finish;
static bareterm_widget_t cb_tp3, cb_tp5, cb_tp13, cb_tp8, cb_tp10, cb_ledd0, cb_ledd4; // cb_ledd0 is for the blinking test
static bareterm_widget_t status_led, lbl_gpio_status;
static char wizard_title_buf[64];


/* The devicetree node identifiers */
#define LED0_NODE     DT_ALIAS(led0)
#define BUTTON_NODE   DT_ALIAS(sw0)
#define SPI_NODE      DT_NODELABEL(spi0)
#define GPIO0_NODE    DT_NODELABEL(gpio0)
#define UART_NODE     DT_CHOSEN(zephyr_console)
#define I2C_NODE      DT_NODELABEL(i3c0)

// --- LED Blinking Test Resources ---
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static const struct device *gpio0_dev = DEVICE_DT_GET(GPIO0_NODE);


static void blink_timer_expiry_function(struct k_timer *timer_id);
K_TIMER_DEFINE(led_blink_timer, blink_timer_expiry_function, NULL);

static void blink_timer_expiry_function(struct k_timer *timer_id)
{
    gpio_pin_toggle_dt(&led);
}

void testfunction_led(void) {

    // Configure the GPIO and start the blink timer
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (!gpio_is_ready_dt(&led)) {
        //report_result("LED", false);
        return;
    }
            k_timer_start(&led_blink_timer, K_MSEC(500), K_MSEC(500));

}

int testfunction_gpio(void) {

    int ret;

    ret = gpio_pin_configure(gpio0_dev, 8, GPIO_OUTPUT);
    if (ret != 0) {
        return(ret);
    }

    ret = gpio_pin_configure(gpio0_dev, 7, GPIO_INPUT);
    if (ret != 0) {
        return(ret);
    }

    ret = gpio_pin_set(gpio0_dev, 8, 1);
    if (ret != 0) {
        return(ret);
    }

    k_sleep(K_MSEC(10)); 

    int value = gpio_pin_get(gpio0_dev, 7); 
    if (value != 1) {
        return -EIO;
    }

    ret = gpio_pin_set(gpio0_dev, 8, 0); 
    if (ret != 0) {
        return ret;
    }

    k_sleep(K_MSEC(10)); 

    value = gpio_pin_get(gpio0_dev, 7); 
    if (value != 0) {
        return -EIO;
    }

    return 0;
    
}

// --- Wizard Navigation Callbacks ---

static void on_checkbox_toggle(bareterm_widget_t *cb, unsigned char state);

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
    // Ensure timer is stopped on finish
    k_timer_stop(&led_blink_timer);
    // Reset the step counter for the next time we enter this screen
    current_step = WIZARD_STEP_INTRO;

    // Also reset the state of the checkboxes
    tp3_vbus_checked = false;
    tp5_vin_checked = false;
    tp13_vdd18_checked = false;
    tp8_vreg1_checked = false;
    tp10_vcore_checked = false;
    cb_ledd0_checked = false;
    cb_ledd4_checked = false;


    // Navigate back to the main screen
    bareterm_screen_manager_set_active(main_screen_get());
}

static void on_checkbox_toggle(bareterm_widget_t *cb, unsigned char state) {
    if (cb == &cb_tp3) {
        tp3_vbus_checked = state;
    } else if (cb == &cb_tp5) {
        tp5_vin_checked = state;
    } else if (cb == &cb_tp13) {
        tp13_vdd18_checked = state;
    } else if (cb == &cb_tp8) {
        tp8_vreg1_checked = state;
    } else if (cb == &cb_tp10) {
        tp10_vcore_checked = state;
    } else if (cb == &cb_ledd4) {
        cb_ledd4_checked = state;
    } else if (cb == &cb_ledd0) {
        cb_ledd0_checked = state;
    }

    // Reload the screen. This is necessary to re-evaluate the state
    // of the 'Next' button, which is disabled until all boxes are checked.
    bareterm_screen_manager_set_active(test_screen_get());
}

// --- Screen Lifecycle Functions ---

// on_enter is the "blueprint" for the screen. It runs every time the screen
// is activated, and it uses the `current_step` variable to decide what to draw.
static void on_test_screen_enter(bareterm_screen_t *screen) {
    // Stop the blink timer by default. It will be restarted only if we are on the LED test step.
    // This is a robust way to manage the timer's lifecycle.
    k_timer_stop(&led_blink_timer);

    bareterm_draw_box(1, 1, 80, 30, " Test Procedure ");

    // Initialize the progress bar to show the current step. This is drawn on every step.
    bareterm_progressbar_init(&progress_bar, 5, 26, 40, 1, current_step + 1, WIZARD_STEP_COUNT, bareterm_GREEN, bareterm_BLACK, bareterm_STYLE_NONE);

    switch (current_step) {
        case WIZARD_STEP_INTRO:
            snprintf(wizard_title_buf, sizeof(wizard_title_buf), "Step %d: Introduction", current_step + 1);
            bareterm_label_init(&lbl_title, 5, 3, wizard_title_buf, bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
            bareterm_textbox_init(&memo_instructions, 5, 5, 70, 5, "To start tests please follow the instructions below. For questions, contact ismail.tasdemir@analog.com or gokhan.bektas@analog.com.", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);
            
            // Navigation for Step 1
            bareterm_button_init(&btn_next, 66, 25, 12, 3, "[ Next ]", on_next_click);
            break;

        case WIZARD_STEP_INSPECTION:
            snprintf(wizard_title_buf, sizeof(wizard_title_buf), "Step %d: Visual Inspection", current_step + 1);
            bareterm_label_init(&lbl_title, 5, 3, wizard_title_buf, bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
            bareterm_textbox_init(&memo_instructions, 5, 5, 70, 8, "Check for solder bridges, missing components, and correct shunt placement. Verify jumper settings against the Jumper Settings Table and confirm shunt positions using the 3D board images.", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);

            // Navigation for Step 2
            bareterm_button_init(&btn_back, 52, 25, 12, 3, "[ Back ]", on_back_click);
            bareterm_button_init(&btn_next, 66, 25, 12, 3, "[ Next ]", on_next_click);
            break;

        case WIZARD_STEP_VOLTAGES:
            snprintf(wizard_title_buf, sizeof(wizard_title_buf), "Step %d: Voltage Checks", current_step + 1);
            bareterm_label_init(&lbl_title, 5, 3, wizard_title_buf, bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
            bareterm_textbox_init(&memo_instructions, 5, 5, 70, 3, "Use a multimeter to measure the voltages at the test points shown below. Verify that each reading matches the expected value in the table.", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);

            // Add checkboxes for each voltage measurement
            int y_pos = 10;
            bareterm_checkbox_init(&cb_tp3,  7, y_pos   , "TP3  - VBUS     5V   (4.5V  - 5.5V)",       tp3_vbus_checked,   on_checkbox_toggle);
            bareterm_checkbox_init(&cb_tp5,  7, y_pos+2 , "TP5  - VIN      1.5V (1.425 – 1.575V)",  tp5_vin_checked,    on_checkbox_toggle);
            bareterm_checkbox_init(&cb_tp8,  7, y_pos+4 , "TP8  – VREG1    1V   (0.95V – 1.05V)",     tp8_vreg1_checked,  on_checkbox_toggle);
            bareterm_checkbox_init(&cb_tp10, 7, y_pos+6 , "TP10 - VCORE    1V   (0.95V – 1.05V)",     tp10_vcore_checked, on_checkbox_toggle);
            bareterm_checkbox_init(&cb_tp13, 7, y_pos+8 , "TP13 – VDD18    1.8V (1.71V – 1.89V)",   tp13_vdd18_checked, on_checkbox_toggle);
            
            bareterm_checkbox_init(&cb_ledd4, 7, y_pos+10, "Observe that the Power LED – D4 is illuminated.",   cb_ledd4_checked, on_checkbox_toggle);
            bareterm_statusindicator_init(&status_led, 5, y_pos+10, "", bareterm_BLUE);

            // Navigation for Step 3
            bareterm_button_init(&btn_back, 52, 25, 12, 3, "[ Back ]", on_back_click);

            // Only enable the 'Next' button if all voltages have been checked
            bool all_checked = tp3_vbus_checked && tp5_vin_checked && tp13_vdd18_checked && tp8_vreg1_checked && tp10_vcore_checked && cb_ledd4_checked;
            if (all_checked) {
                bareterm_button_init(&btn_next, 66, 25, 12, 3, "[ Next ]", on_next_click);
            } else {
                bareterm_button_init(&btn_next, 66, 25, 12, 3, "[ ... ]", NULL);
            }
            break;
        case WIZARD_STEP_LED_TEST:
            snprintf(wizard_title_buf, sizeof(wizard_title_buf), "Step %d: LED Test", current_step + 1);
            bareterm_label_init(&lbl_title, 5, 3, wizard_title_buf, bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
            bareterm_textbox_init(&memo_instructions, 5, 5, 70, 3, "Verify that the GREEN LED – D0 is blinking at 1Hz. Check the box to confirm.", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);

            testfunction_led();

            // Add a checkbox for user confirmation
            bareterm_checkbox_init(&cb_ledd0, 7, 10, "GREEN LED - D0 is blinking correctly.", cb_ledd0_checked, on_checkbox_toggle);
            bareterm_statusindicator_init(&status_led, 5, 10, "", bareterm_GREEN);

            // Navigation for Step 4
            bareterm_button_init(&btn_back, 52, 25, 12, 3, "[ Back ]", on_back_click);

            // Only enable the 'Next' button if the user has confirmed the blink
            all_checked =  cb_ledd0_checked;
            if (all_checked) {
                bareterm_button_init(&btn_next, 66, 25, 12, 3, "[ Next ]", on_next_click);
            } else {
                bareterm_button_init(&btn_next, 66, 25, 12, 3, "[ ... ]", NULL);
            }
            break;
        case WIZARD_STEP_GPIO_TEST_PREPARE:
            snprintf(wizard_title_buf, sizeof(wizard_title_buf), "Step %d: GPIO Test", current_step + 1);
            bareterm_label_init(&lbl_title, 5, 3, wizard_title_buf, bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
            bareterm_textbox_init(&memo_instructions, 5, 5, 70, 3, "Please remove the GPIO2 and INTB jumper from the JP15 header and connect these pin with a jumper cable on J13 Header(7 and 8). Then click Next to start.", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);
            
            // Navigation for previous step
            bareterm_button_init(&btn_back, 52, 25, 12, 3, "[ Back ]", on_back_click);
            bareterm_button_init(&btn_next, 66, 25, 12, 3, "[ Next ]", on_next_click);
            break;
        case WIZARD_STEP_GPIO_TEST_PERFORM:
            snprintf(wizard_title_buf, sizeof(wizard_title_buf), "Step %d: GPIO Test", current_step + 1);
            bareterm_label_init(&lbl_title, 5, 3, wizard_title_buf, bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
            bareterm_textbox_init(&memo_instructions, 5, 5, 70, 3, "Testing J13 Header GPIO Pins", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);
            
            int test_result = testfunction_gpio();
            if (test_result == 0) {
                bareterm_label_init(&lbl_gpio_status, 7, 10, "GPIO Test: PASSED", bareterm_GREEN, bareterm_BLACK, bareterm_STYLE_BOLD);
            } else {
                bareterm_label_init(&lbl_gpio_status, 7, 10, "GPIO Test: FAILED", bareterm_RED, bareterm_BLACK, bareterm_STYLE_BOLD);
            }

            // Navigation for previous step
            bareterm_button_init(&btn_back, 52, 25, 12, 3, "[ Back ]", on_back_click);
            bareterm_button_init(&btn_next, 66, 25, 12, 3, "[ Next ]", on_next_click);
            break;
        case WIZARD_STEP_COMPLETE:
            snprintf(wizard_title_buf, sizeof(wizard_title_buf), "Step %d: Complete", current_step + 1);
            bareterm_label_init(&lbl_title, 5, 3, wizard_title_buf, bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
            bareterm_textbox_init(&memo_instructions, 5, 5, 70, 3, "The initial hardware check is complete. Press Finish to return to the main menu.", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);

            // Navigation for Step 4
            bareterm_button_init(&btn_back, 52, 25, 12, 3, "[ Back ]", on_back_click);
            bareterm_button_init(&btn_finish, 66, 25, 12, 3, "[ Finish ]", on_finish_click);
            break;

        default:
            // Invalid step, reset to the beginning to be safe
            current_step = WIZARD_STEP_INTRO;
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
