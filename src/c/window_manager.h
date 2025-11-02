#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <pebble.h>

// Initialize and tear down the window manager
void wm_init(void);
void wm_deinit(void);

// Register a named window with the manager. The create function should
// allocate and set up a Window but not push it. The destroy function
// should free the Window. The optional set_text function can be NULL.
// The manager will call create() lazily when showing the window.
bool wm_register(const char *name,
                 Window *(*create_fn)(void),
                 void (*destroy_fn)(Window *),
                 void (*set_text_fn)(const char *));

// Show/hide windows by name
void wm_show_by_name(const char *name, bool animated);
void wm_hide_by_name(const char *name, bool animated);
bool wm_is_shown_by_name(const char *name);

// Convenience for setting text on a named window (if it registered a set_text_fn)
void wm_set_text_by_name(const char *name, const char *text);

void wm_init(void);
void wm_deinit(void);
void wm_show(bool animated);
void wm_hide(bool animated);
bool wm_is_shown(void);
void wm_set_text(const char *text);

#endif // WINDOW_MANAGER_H