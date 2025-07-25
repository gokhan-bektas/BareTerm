// tui_event_queue.h
#ifndef TUI_EVENT_QUEUE_H
#define TUI_EVENT_QUEUE_H

#include <zephyr/kernel.h>
#include "tui_event.h"

#define TUI_EVENT_QUEUE_SIZE 8

extern struct k_msgq tui_event_q;

#endif
