#include <pebble.h>
#include "hcb-pebble.h"

static Window *s_window;
static TextLayer *s_text_layer;
static bool *mobile_ready = false;

/* static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Select");
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Up");
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Down");
} */

/* static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
} */

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text(s_text_layer, "Init");
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_APP_READY);
  Tuple *auth_user_code_tuple = dict_find(iter, MESSAGE_KEY_AUTH_USER_CODE);
  Tuple *auth_access_token_tuple = dict_find(iter, MESSAGE_KEY_AUTH_ACCESS_TOKEN);
  Tuple *auth_refresh_token_tuple = dict_find(iter, MESSAGE_KEY_AUTH_REFRESH_TOKEN);
  if (ready_tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Recieved Ready");
    if (!persist_exists(PERSISTENT_ACCESS_TOKEN)) {
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
    text_layer_set_text(s_text_layer, s_buf);
  }
  if (auth_access_token_tuple && auth_access_token_tuple->value->cstring) {
    static char s_buf[128];
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Access Token: %s", auth_access_token_tuple->value->cstring);
    snprintf(s_buf, sizeof(s_buf), "%s", auth_access_token_tuple->value->cstring);
    persist_write_string(PERSISTENT_ACCESS_TOKEN, auth_access_token_tuple->value->cstring);
  }
   if (auth_refresh_token_tuple && auth_refresh_token_tuple->value->cstring) {
    static char s_buf[128];
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Refresh Token: %s", auth_refresh_token_tuple->value->cstring);
    snprintf(s_buf, sizeof(s_buf), "%s", auth_refresh_token_tuple->value->cstring);
    persist_write_string(PERSISTENT_REFRESH_TOKEN, auth_refresh_token_tuple->value->cstring);
  }
}


static void prv_init(void) {
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);
  s_window = window_create();
  //window_set_click_config_provider(s_window, prv_click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
