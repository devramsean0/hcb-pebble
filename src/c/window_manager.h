/*
 * window_manager.h
 *
 * Copyright (C) 2025 Joe Citrine
 * From: https://github.com/jccit/pebblerail
 *
 * This file is licensed under the GNU General Public License v3.0 (or later).
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <pebble.h>

void window_manager_push_window(Window *window);
void window_manager_destroy_window(Window *window);

/* Removes the oldest window from the stack to free up memory */
void window_manager_free_oldest();

/* Ensure we have enough space for the requested window
 * This will free old windows if necessary
 */
void *wm_alloc(size_t size);