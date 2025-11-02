#include "wm_text_window.h"
#include <string.h>

static Window *s_window = NULL;
static TextLayer *s_text_layer = NULL;
static char s_pending_text[128];
static bool s_has_pending_text = false;

static void prv_window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  s_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  if (s_has_pending_text) {
    text_layer_set_text(s_text_layer, s_pending_text);
    s_has_pending_text = false;
  } else {
    text_layer_set_text(s_text_layer, "...");
  }
  layer_add_child(root, text_layer_get_layer(s_text_layer));
}

static void prv_window_unload(Window *window) {
  if (s_text_layer) {
    text_layer_destroy(s_text_layer);
    s_text_layer = NULL;
  }
}

Window *wm_text_window_create(void) {
  if (s_window) return s_window; // already created
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  return s_window;
}

void wm_text_window_destroy(Window *window) {
  if (!s_window) return;
  // If window is shown, remove it from stack
  // Caller/manager will have removed the window_stack entry prior to calling destroy
  if (s_window == window) {
    if (s_text_layer) {
      text_layer_destroy(s_text_layer);
      s_text_layer = NULL;
    }
    window_destroy(s_window);
    s_window = NULL;
  }
  s_has_pending_text = false;
}

void wm_text_window_set_text(const char *text) {
  if (s_text_layer) {
    text_layer_set_text(s_text_layer, text ? text : "");
  } else {
    // store pending text so it is applied when window loads
    if (text) {
      strncpy(s_pending_text, text, sizeof(s_pending_text) - 1);
      s_pending_text[sizeof(s_pending_text) - 1] = '\0';
      s_has_pending_text = true;
    }
  }
}
