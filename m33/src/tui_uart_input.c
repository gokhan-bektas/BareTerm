#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include "tui_event.h"

#define UART_DEVICE_NODE DT_NODELABEL(uart0)  // Change if using uart1 or another

static const struct device *uart_dev;

static void uart_cb(const struct device *dev, void *user_data)
{
    uint8_t c;

    while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
        if (uart_irq_rx_ready(dev)) {
            int recv = uart_fifo_read(dev, &c, 1);
            if (recv > 0) {
                // Feed into your TUI input system
                tui_input_feed_byte((char)c);  // This will turn into tui_event_t
            }
        }
    }
}

void tui_uart_input_init(void)
{
    uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
    if (!device_is_ready(uart_dev)) {
        printk("UART not ready!\n");
        return;
    }

    uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);
    uart_irq_rx_enable(uart_dev);
}