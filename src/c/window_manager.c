/*
 * window_manager.c
 *
 * Copyright (C) 2025 Joe Citrine
 * From: https://github.com/jccit/pebblerail
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "window_manager.h"

#include "utils.h"
#include <stdlib.h>

#ifdef PBL_PLATFORM_APLITE
#define MEMORY_SAFETY_MARGIN 1800
#define MIN_WINDOWS 1
#define MAX_WINDOWS 5
#else
#define MEMORY_SAFETY_MARGIN 2048
#define MIN_WINDOWS 2
#define MAX_WINDOWS 8
#endif

static Window *s_windows[MAX_WINDOWS];
static uint8_t s_window_count = 0;

void window_manager_push_window(Window *window) {
  LOG_DEBUG("WM: Pushing window");

  if (s_window_count >= MAX_WINDOWS) {
    window_manager_free_oldest();
  }

  s_windows[s_window_count++] = window;

  LOG_DEBUG_VERBOSE("WM: New window count: %d", s_window_count);

  check_free_memory();

  window_stack_push(window, true);
}

void window_manager_destroy_window(Window *window) {
  LOG_DEBUG("WM: Destroying window");

  for (uint8_t i = 0; i < s_window_count; ++i) {
    if (s_windows[i] == window) {
      /* shift array to left after removing */
      for (uint8_t j = i; j < s_window_count - 1; ++j) {
        s_windows[j] = s_windows[j + 1];
      }
      s_window_count--;
      break;
    }
  }

  LOG_DEBUG_VERBOSE("WM: Window count after free: %d", s_window_count);

  window_stack_remove(window, false);
}

void window_manager_free_oldest() {
  if (s_window_count <= MIN_WINDOWS) {
    LOG_WARN("WM: Not enough windows to free");
    return;
  }

  LOG_DEBUG("WM: Freeing oldest window");
  window_manager_destroy_window(s_windows[0]);
}

void *wm_alloc(size_t requested_size) {
  LOG_DEBUG_VERBOSE("WM: Allocating %zu bytes", requested_size);

  size_t needed_memory = requested_size + MEMORY_SAFETY_MARGIN;
  size_t free_memory = check_free_memory();

  if (free_memory < needed_memory) {
    LOG_DEBUG("WM: Not enough free memory, freeing old windows");
    window_manager_free_oldest();
  }

  return malloc(requested_size);
}