#include "window_manager.h"
#include <string.h>
#include <stdlib.h>

#define WM_MAX_WINDOWS 8

typedef struct {
  const char *name;
  Window *window;
  Window *(*create_fn)(void);
  void (*destroy_fn)(Window *);
  void (*set_text_fn)(const char *);
  bool shown;
} wm_entry_t;

static wm_entry_t s_entries[WM_MAX_WINDOWS];
static bool s_initialized = false;

static int find_index_by_name(const char *name) {
  for (int i = 0; i < WM_MAX_WINDOWS; ++i) {
    if (s_entries[i].name && strcmp(s_entries[i].name, name) == 0) return i;
  }
  return -1;
}

static int find_free_slot(void) {
  for (int i = 0; i < WM_MAX_WINDOWS; ++i) {
    if (s_entries[i].name == NULL) return i;
  }
  return -1;
}

void wm_init(void) {
  if (s_initialized) return;
  memset(s_entries, 0, sizeof(s_entries));
  s_initialized = true;
}

void wm_deinit(void) {
  if (!s_initialized) return;
  // Destroy any created windows and call destroy_fns
  for (int i = 0; i < WM_MAX_WINDOWS; ++i) {
    if (s_entries[i].name) {
      if (s_entries[i].shown && s_entries[i].window) {
        window_stack_remove(s_entries[i].window, false);
        s_entries[i].shown = false;
      }
      if (s_entries[i].window && s_entries[i].destroy_fn) {
        s_entries[i].destroy_fn(s_entries[i].window);
        s_entries[i].window = NULL;
      }
      // free name if it was strdup'd; we only strdup for safety when registering
      free((void *)s_entries[i].name);
      s_entries[i].name = NULL;
    }
  }
  s_initialized = false;
}

bool wm_register(const char *name,
                 Window *(*create_fn)(void),
                 void (*destroy_fn)(Window *),
                 void (*set_text_fn)(const char *)) {
  if (!s_initialized || !name || !create_fn) return false;
  if (find_index_by_name(name) >= 0) return false; // already registered
  int slot = find_free_slot();
  if (slot < 0) return false;
  // copy name so caller may use temporary strings
  size_t nlen = strlen(name) + 1;
  char *copy = (char *)malloc(nlen);
  if (!copy) return false;
  memcpy(copy, name, nlen);
  s_entries[slot].name = copy;
  s_entries[slot].create_fn = create_fn;
  s_entries[slot].destroy_fn = destroy_fn;
  s_entries[slot].set_text_fn = set_text_fn;
  s_entries[slot].window = NULL;
  s_entries[slot].shown = false;
  return true;
}

void wm_show_by_name(const char *name, bool animated) {
  if (!s_initialized || !name) return;
  int idx = find_index_by_name(name);
  if (idx < 0) return;
  wm_entry_t *e = &s_entries[idx];
  if (!e->window) {
    e->window = e->create_fn();
  }
  if (e->window && !e->shown) {
    window_stack_push(e->window, animated);
    e->shown = true;
  }
}

void wm_hide_by_name(const char *name, bool animated) {
  if (!s_initialized || !name) return;
  int idx = find_index_by_name(name);
  if (idx < 0) return;
  wm_entry_t *e = &s_entries[idx];
  if (e->window && e->shown) {
    window_stack_remove(e->window, animated);
    e->shown = false;
  }
}

bool wm_is_shown_by_name(const char *name) {
  if (!s_initialized || !name) return false;
  int idx = find_index_by_name(name);
  if (idx < 0) return false;
  return s_entries[idx].shown;
}

void wm_set_text_by_name(const char *name, const char *text) {
  if (!s_initialized || !name) return;
  int idx = find_index_by_name(name);
  if (idx < 0) return;
  wm_entry_t *e = &s_entries[idx];
  if (e->set_text_fn) e->set_text_fn(text);
}
