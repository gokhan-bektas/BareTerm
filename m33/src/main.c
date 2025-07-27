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
#include "tui.h"
#include "tui_backend.h"
#include "tui.h"
#include "tui_input.h"	
#include "tui_uart_input.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)


/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
	int ret;
	bool led_state = true;

	tui_init();

    const struct device *uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
    if (!device_is_ready(uart)) {
        return;
    }

    tui_uart_input_init(uart);

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

tui_backend_puts("\x1B[?1000h"); // Basic mouse click tracking

while (1) {
    tui_event_t evt;
    if (tui_poll_event(&evt)) {
        //tui_move_cursor(1, 15);
		
        switch (evt.type) {
            case TUI_EVT_KEY:
                tui_printf("Key: '%c' (0x%02X)    ", evt.key.ch, evt.key.ch);
                break;
			case TUI_EVT_ARROW:
				tui_printf("Arrow:  %d    ", evt.key.code);
				break;
            case TUI_EVT_MOUSE:
                tui_printf("Mouse: %s at (%d, %d), button %d     ",
                           evt.mouse.pressed ? "DOWN" : "UP",
                           evt.mouse.x, evt.mouse.y,
                           evt.mouse.button);
                break;
        }
    }
    k_msleep(10);
}


	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return 0;
		}



		led_state = !led_state;
		printf("LED state: %s\n", led_state ? "ON" : "OFF");

		tui_set_color(TUI_WHITE, TUI_BLUE, TUI_STYLE_NONE);
		tui_puts("Plain white on blue");

		tui_set_color(TUI_YELLOW, TUI_RED, TUI_STYLE_BOLD);
		tui_puts("Bold yellow on red");

		tui_set_color(TUI_GREEN, TUI_BLACK, TUI_STYLE_UNDERLINE);
		tui_puts("Underlined green on black");

		tui_reset_color();
		tui_move_cursor(1, 3);
		tui_puts("This is default color again");

		tui_set_color(TUI_WHITE, TUI_BLUE, TUI_STYLE_BOLD);
		tui_draw_box(5, 3, 30, 7, " Menu ");
		tui_reset_color();

		tui_move_cursor(7, 5);
		tui_puts("Option 1");
		tui_move_cursor(7, 6);
		tui_puts("Option 2");

		k_msleep(SLEEP_TIME_MS);


		printf("\x1B[2J\x1B[H\x1B[44;37mThis is a test\x1B[0m\n");

        tui_clear_screen();
        tui_backend_puts("Screen cleared\n");
        tui_printf("System ready on port %d\n", 3);
	}
	return 0;
}
