#include <pebble.h>
#include "hcb-pebble.h"
#include "window_manager.h"
#include "wm_text_window.h"

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_APP_READY);
  Tuple *auth_user_code_tuple = dict_find(iter, MESSAGE_KEY_AUTH_USER_CODE);
  Tuple *auth_access_token_tuple = dict_find(iter, MESSAGE_KEY_AUTH_ACCESS_TOKEN);
  Tuple *auth_refresh_token_tuple = dict_find(iter, MESSAGE_KEY_AUTH_REFRESH_TOKEN);

  if (ready_tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Recieved Ready");
    if (!persist_exists(ACCESS_TOKEN)) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Access Token doesn't exist");
      DictionaryIterator *out;
      app_message_outbox_begin(&out);
      dict_write_cstring(out, MESSAGE_KEY_REQUEST, "BEGIN_AUTH");
      app_message_outbox_send();
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Sent Auth Init Req");
      return;
    }
    return;
  }

  if (auth_user_code_tuple && auth_user_code_tuple->value->cstring) {
    static char s_buf[128];
    snprintf(s_buf, sizeof(s_buf), "%s", auth_user_code_tuple->value->cstring);
    // Use centralized manager: show the registered "auth" window and set text
    if (!wm_is_shown_by_name("auth")) {
      wm_show_by_name("auth", true);
    }
    wm_set_text_by_name("auth", s_buf);
  }

  if (auth_access_token_tuple && auth_access_token_tuple->value->cstring) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Access Token: %s", auth_access_token_tuple->value->cstring);
    persist_write_string(ACCESS_TOKEN, auth_access_token_tuple->value->cstring);
  }

  if (auth_refresh_token_tuple && auth_refresh_token_tuple->value->cstring) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Refresh Token: %s", auth_refresh_token_tuple->value->cstring);
    persist_write_string(REFRESH_TOKEN, auth_refresh_token_tuple->value->cstring);
  }
}

static void prv_init(void) {
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);
  // Initialize centralized window manager and register windows
  wm_init();
  // Register a simple text window under the name "auth"
  wm_register("auth", wm_text_window_create, wm_text_window_destroy, wm_text_window_set_text);
  // preserve original behavior: push the auth window on init
  wm_show_by_name("auth", true);
}

static void prv_deinit(void) {
  wm_deinit();
}

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window");

  app_event_loop();
  prv_deinit();
}