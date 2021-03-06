#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

#define PERSIST_KEY_STEPS 52

static int s_worker_steps;

static void get_step_count() {
  // Get the data from Persistent Storage
  s_worker_steps = persist_read_int(PERSIST_KEY_STEPS);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {


  bool running = app_worker_is_running();

  if (running){
    get_step_count();

    static char step_count[] = "9999";
    snprintf(step_count, sizeof("9999"), "%d", s_worker_steps);
    
    text_layer_set_text(text_layer, step_count);
  }else{
    text_layer_set_text(text_layer, "Not Running");
  }

  
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  AppWorkerResult result = app_worker_launch();
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
