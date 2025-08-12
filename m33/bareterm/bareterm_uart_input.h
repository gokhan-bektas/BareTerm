#ifndef bareterm_UART_INPUT_H
#define bareterm_UART_INPUT_H

#include <zephyr/device.h>  // ✅ Needed to define struct device
#include <zephyr/drivers/uart.h>

void bareterm_uart_input_init(const struct device *dev);

#endif