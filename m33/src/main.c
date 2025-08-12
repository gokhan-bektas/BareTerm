/*
 * Copyright (c) 2016 Intel Corporation
 * Portions Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>

#include <bareterm.h>
#include <bareterm_widget.h>



/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)


/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static bareterm_widget_t btn1, btn2;
static bareterm_widget_t memo;
static bareterm_widget_t lbl1;
static bareterm_widget_t lbl2;
static bareterm_widget_t bar;
static bareterm_widget_t cb1, cb2;

static bareterm_widget_t rg;
static const char *choices[] = { "SPI", "I2C", "UART" };

static bareterm_widget_t input;
static char buf[64] = "";

void on_submit(bareterm_widget_t *ti) {
    // Copy out and display
    char out[64];
    bareterm_textinput_get_text(ti, out, sizeof(out));
    bareterm_move_cursor(1, 20);
    bareterm_printf("You entered: %s", out);
}

// Callback for button press
static void on_button_click1(void *w) {
    // Update the label
    bareterm_label_set_text(&lbl1, "Clicked Button 1!");
    bareterm_label_set_color(&lbl1, bareterm_YELLOW, bareterm_BLUE, bareterm_STYLE_BOLD);

	bareterm_progressbar_set_value(&bar, /*new_value=*/ bar.value - 1);

    // Redraw everything
    bareterm_draw_all_widgets();
}
// Callback for button press
static void on_button_click2(void *w) {
    // Update the label
    bareterm_label_set_text(&lbl1, "Clicked Button 2!");
    bareterm_label_set_color(&lbl1, bareterm_YELLOW, bareterm_GREEN, bareterm_STYLE_BOLD);

	bareterm_progressbar_set_value(&bar, /*new_value=*/ bar.value + 1);
    // Redraw everything
    bareterm_draw_all_widgets();

}


void on_toggle(bareterm_widget_t *cb, unsigned char state) {
    bareterm_move_cursor(10, 20);
    bareterm_printf("Checkbox is now %s   ", state ? "☑️" : "☐");

	if (cb->checked) {
		bareterm_label_set_text(&lbl2, "Checkbox enabled");
		bareterm_backend_puts("\x1B[?1049h");  // Push into alternate screen
		//bareterm_draw_box(6, 24, 10, 3, " TEST");
		bareterm_modalpopup_show("Checkbox Enabled", "You have enabled the checkbox feature.");
	} else {
		bareterm_label_set_text(&lbl2, "Checkbox disabled");
		bareterm_backend_puts("\x1B[?1049h");  // Push into alternate screen
	}	
		
	// draw your dialog and whatever else
	bareterm_backend_puts("\x1B[?1049l");  // Pop back to the original screen

}

void on_choice_change(bareterm_widget_t *w, int sel) {
    bareterm_move_cursor(4, 18);
    bareterm_printf("Selected: %s   ", choices[sel]);
}

int test_draw_welcomescreen() {

	bareterm_clear_screen();
	bareterm_draw_box(1, 1, 50, 30, " MAX32657 EVKit Test Tool v0.1.0");

	bareterm_textbox_init(&memo,
					3, 8,
					47, 5,
					"This is the automated production test tool for "
					"MAX32657 Rev-C EVKit. Use mouse to interact."
					"                                           "
					"Click START Button to begin.",
					bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);

    bareterm_button_init(&btn1,
                    15, 18,
                    20, 3,
                    " START ",
                    on_button_click1);


}

int main(void)
{
	int ret;
	bool led_state = true;

	bareterm_init();

    const struct device *uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
    if (!device_is_ready(uart)) {
        return;
    }

    bareterm_uart_input_init(uart);

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

			// Hide cursor
		bareterm_backend_write("\x1B[?25l", 6);

		// Show cursor
		//bareterm_backend_write("\x1B[?25h", 6);

    bareterm_backend_puts("\x1B[?1000h"); // Basic mouse click tracking

	//test_draw_welcomescreen();

    bareterm_clear_screen();

    // Draw a static label
    //bareterm_move_cursor(2, 2);
    //bareterm_puts("Press the button:");

	bareterm_draw_box(1, 1, 50, 30, " MAX32657 EVKit Test Tool v0.1.0");
	bareterm_label_init(&lbl1,
				15, 4,
				"Welcome",
				bareterm_WHITE,   // foreground
				bareterm_BLUE,      // background
				bareterm_STYLE_BOLD);

    // 2) Initialize it at position (col=2, row=2) with your text
	bareterm_label_init(&lbl2,
				14, 24,
				"Click somewhere to start",
				bareterm_YELLOW,   // foreground
				bareterm_BLUE,      // background
				bareterm_STYLE_BOLD);

    // Create a button widget

    bareterm_button_init(&btn1,
                    4, 6,
                    20, 3,
                    "[ Button1 ]",
                    on_button_click1);
    bareterm_button_init(&btn2,
                    26, 6,
                    20, 3,
                    "[ Button2 ]",
                    on_button_click2);



	bareterm_textbox_init(&memo,
					3, 10,
					36, 5,
					"To start tests please follow the instructions below:"
					"Here are the instructions. Under construction...",
					bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);

    bareterm_progressbar_init(&bar,
                         4,  15,
                         38, 1,
                         30, 100,
                         bareterm_GREEN, bareterm_BLACK, bareterm_STYLE_NONE);

    bareterm_checkbox_init(&cb1,
                      12, 27,
                      "Enable feature 1",
                      false,  //initial=
                      on_toggle);

    bareterm_checkbox_init(&cb2,
                      12, 28,
                      "Enable feature 2",
                      false,  //initial=
                      on_toggle);


    bareterm_radiogroup_init(&rg,
        4, 19,
        choices, 3,   // labels + count
        1,            // start with “Green”
        on_choice_change);

	/*buf[0] = 'A'; // Initialize text input buffer
	buf[1] = 'B'; // Initialize text input buffer
    bareterm_textinput_init(&input,
                       30, 2,
                       27,
                       buf, sizeof(buf),
                       on_submit);
	*/				   

	// Initial draw
    bareterm_draw_all_widgets();

    // Main event loop
    while (1) {
        bareterm_event_t evt;
        if (bareterm_poll_event(&evt)) {

			switch (evt.type) {
				case bareterm_EVT_KEY:
					//bareterm_printf("Key: '%c' (0x%02X)    ", evt.key.ch, evt.key.ch);
					break;
				case bareterm_EVT_ARROW:
					//bareterm_printf("Arrow:  %d    ", evt.key.code);
					break;
				case bareterm_EVT_MOUSE:
					//bareterm_printf("Mouse: %s at (%d, %d), button %d     ",
					//		evt.mouse.pressed ? "DOWN" : "UP",
					//		evt.mouse.x, evt.mouse.y,
					//		evt.mouse.button);
					break;
			}

            bareterm_dispatch_event(&evt);
            bareterm_draw_all_widgets();
        }
        k_msleep(10);
    }

	return 0;
}
