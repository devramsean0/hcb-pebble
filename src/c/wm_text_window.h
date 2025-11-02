#ifndef WM_TEXT_WINDOW_H
#define WM_TEXT_WINDOW_H

#include <pebble.h>

// Lightweight text-window helper used with the centralized window manager.
// The create/destroy/set_text functions match the manager expectations.
Window *wm_text_window_create(void);
void wm_text_window_destroy(Window *window);
void wm_text_window_set_text(const char *text);

#endif // WM_TEXT_WINDOW_H
