// ============================
// File: tui_uart_input.c
// ============================
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include "tui_input.h"

static const struct device *uart_dev;

static void uart_cb(const struct device *dev, void *user_data) {
    ARG_UNUSED(user_data);

    while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
        if (uart_irq_rx_ready(dev)) {
            uint8_t c;
            while (uart_fifo_read(dev, &c, 1)) {
                tui_input_feed((char)c);
            }
        }
    }
}

void tui_uart_input_init(const struct device *dev) {
    uart_dev = dev;
    uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);
    uart_irq_rx_enable(uart_dev);
}