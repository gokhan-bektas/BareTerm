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
#include <bareterm_screen_manager.h>
#include "main_screen.h"
#include "test_screen.h"

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


	bareterm_init();

    const struct device *uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
    if (!device_is_ready(uart)) { // Note: Corrected return type
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

    // Initialize the screen manager and set the initial screen
    bareterm_screen_manager_init();
    //bareterm_screen_manager_set_active(main_screen_get());
    bareterm_screen_manager_set_active(test_screen_get());
    // Main event loop
    while (1) {
        bareterm_event_t evt;
        if (bareterm_poll_event(&evt)) {
            bareterm_dispatch_event(&evt);
            bareterm_draw_all_widgets();
        }
        k_msleep(10);
    }

	return 0;
}
