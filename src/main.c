#include <pebble.h>
#include <PDUtils.h>
#include "main.h"

#define MENU_SECTIONS 1
#define NUMBER_OF_TEA_TYPES 6
#define MENU_ITEMS NUMBER_OF_TEA_TYPES
#define DEFAULT_STEEP_INTERVAL 0.5
#define BLACK_TEA_STEEP_SK 1
#define GREEN_TEA_STEEP_SK 2
#define HERBAL_TEA_STEEP_SK 3
#define OOLONG_TEA_STEEP_SK 4
#define WHITE_TEA_STEEP_SK 5
#define YELLOW_TEA_STEEP_SK 6

Window *main_window;
MenuLayer *menu_layer;
Window *timer_window;
Layer *timer_layer;
TextLayer *tea_name_layer;
TextLayer *count_down_layer;
TextLayer *temperature_layer;
TextLayer *start_help_text;

time_t startTime;
double steepTime;
time_t endTime;
bool countdownActive = false;

struct Tea {
    char *name;
    char *temperature;
    double steepTime;
    int storageKey;
};

struct Tea teaTypes[NUMBER_OF_TEA_TYPES];
struct Tea *activeTea;


void deallocate_timer_window() {

  if(start_help_text != NULL) {
    text_layer_destroy(start_help_text);
  }

  if(temperature_layer != NULL) {
    text_layer_destroy(temperature_layer);
  }

  if(count_down_layer != NULL) {
    text_layer_destroy(count_down_layer);
  }

  if(tea_name_layer != NULL) {
    text_layer_destroy(tea_name_layer);
  }

  if(timer_layer != NULL) {
    layer_destroy(timer_layer);
  }

  if(timer_window != NULL) {
    window_destroy(timer_window);
  }

}

uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return MENU_SECTIONS;
}


uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_ITEMS;
}

int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}


void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  return menu_cell_basic_header_draw(ctx, cell_layer, "Select tea type");
}


void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

  int tea_index = cell_index->row;
  menu_cell_basic_draw(ctx, cell_layer, teaTypes[tea_index].name, teaTypes[tea_index].temperature, NULL);

}

void timer_update_handler(struct tm *tick_time, TimeUnits units_changed) {

  time_t time_passed = p_mktime(tick_time) - startTime;

  time_t time_left = (endTime - startTime - time_passed);

  update_time_layer(time_left);

  if(time_left <= 0) {

    tick_timer_service_unsubscribe();

    uint32_t const segments[] = { 200, 100, 200 };
    VibePattern pat = {
      .durations = segments,
      .num_segments = ARRAY_LENGTH(segments),
    };
    vibes_enqueue_custom_pattern(pat);
  }

}

void update_time_layer(time_t time) {

  char *time_string = "0:00";
  int minutes = (int)time / 60;
  int seconds = (int)time % 60;

  snprintf(time_string, 5, "%01d:%02d", minutes, seconds);

  text_layer_set_text(count_down_layer, time_string);
  layer_mark_dirty(text_layer_get_layer(count_down_layer));
}


void select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
    if(countdownActive) {
      return;
    }

    activeTea->steepTime = steepTime;
    persist_write_int(activeTea->storageKey, steepTime * 100);

    countdownActive = true;
    startTime = time(NULL);
    endTime = startTime + (steepTime * 60);
    text_layer_set_text(start_help_text, "Back button to stop");
    layer_mark_dirty(text_layer_get_layer(start_help_text));
    tick_timer_service_subscribe(SECOND_UNIT, timer_update_handler);
}

void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {

    if(countdownActive) {
      return;
    }

    steepTime += (double)DEFAULT_STEEP_INTERVAL;
    update_time_layer(steepTime * 60);

}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {

    if(countdownActive) {
      return;
    }

    if(steepTime * 60 > (double)DEFAULT_STEEP_INTERVAL * 60) {
      steepTime -= (double)DEFAULT_STEEP_INTERVAL;
      update_time_layer(steepTime * 60);
    }
}

void config_provider(Window *window) {
     window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
     window_single_click_subscribe(BUTTON_ID_BACK, back_single_click_handler);
     window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
     window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);

}


void back_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
    countdownActive = false;
    activeTea = NULL;
    tick_timer_service_unsubscribe();
    deallocate_timer_window();
    window_stack_pop(true);
}

void show_timer(const char *tea_name, const char *temperature_text, double steep_time) {

  timer_window = window_create();
  Layer *window_layer = window_get_root_layer(timer_window);
  GRect bounds = layer_get_bounds(window_layer);

  timer_layer = layer_create(bounds);

  tea_name_layer = text_layer_create(GRect(0, 0, bounds.size.w, 35));
  text_layer_set_text(tea_name_layer, tea_name);
  text_layer_set_size(tea_name_layer, GSize(bounds.size.w, 35));
  text_layer_set_text_alignment(tea_name_layer, GTextAlignmentCenter);
  text_layer_set_font(tea_name_layer,fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));

  temperature_layer = text_layer_create(GRect(0, 36, bounds.size.w, 18));
  text_layer_set_text(temperature_layer, temperature_text);
  text_layer_set_size(temperature_layer, GSize(bounds.size.w, 18));
  text_layer_set_text_alignment(temperature_layer, GTextAlignmentCenter);
  text_layer_set_font(temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));

  start_help_text = text_layer_create(GRect(0, bounds.size.h - 16, bounds.size.w, 16));
  text_layer_set_text(start_help_text, "Select button to start");
  text_layer_set_size(start_help_text, GSize(bounds.size.w, 16));
  text_layer_set_text_alignment(start_help_text, GTextAlignmentCenter);
  text_layer_set_font(start_help_text, fonts_get_system_font(FONT_KEY_GOTHIC_14));

  char *time_string = "0:00";
  int minutes = (int)(steep_time * 60) / 60;
  int seconds = (int)(steep_time * 60) % 60;

  snprintf(time_string, 5, "%01d:%02d", minutes, seconds);

  count_down_layer = text_layer_create(GRect(0, 58, bounds.size.w, 45));
  text_layer_set_text(count_down_layer, time_string);
  text_layer_set_size(count_down_layer, GSize(bounds.size.w, 45));
  text_layer_set_text_alignment(count_down_layer, GTextAlignmentCenter);
  text_layer_set_font(count_down_layer,fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));


  layer_add_child(timer_layer, text_layer_get_layer(tea_name_layer));
  layer_add_child(timer_layer, text_layer_get_layer(temperature_layer));
  layer_add_child(timer_layer, text_layer_get_layer(count_down_layer));
  layer_add_child(timer_layer, text_layer_get_layer(start_help_text));
  layer_add_child(window_layer, timer_layer);

  window_stack_push(timer_window, true);

  steepTime = steep_time;

  window_set_click_config_provider(timer_window, (ClickConfigProvider) config_provider);

}


void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {

  int tea_index = cell_index->row;
  activeTea = &teaTypes[tea_index];
  show_timer(teaTypes[tea_index].name, teaTypes[tea_index].temperature, teaTypes[tea_index].steepTime);

}

double read_key_or_default(int key, double default_val) {

  return persist_exists(key) ? (double)persist_read_int(key) / 100.0 : default_val;
}


void handle_init(void) {

  teaTypes[0].name = "Black";
  teaTypes[0].temperature = "100ºC / 210ºF";
  teaTypes[0].storageKey = BLACK_TEA_STEEP_SK;
  teaTypes[0].steepTime = read_key_or_default(BLACK_TEA_STEEP_SK, 2.50);

  teaTypes[1].name = "Green";
  teaTypes[1].temperature = "75-80ºC / 167-176ºF";
  teaTypes[1].storageKey = GREEN_TEA_STEEP_SK;
  teaTypes[1].steepTime = read_key_or_default(GREEN_TEA_STEEP_SK, 1.50);

  teaTypes[2].name = "Herbal";
  teaTypes[2].temperature = "100ºC / 210ºF";
  teaTypes[2].storageKey = HERBAL_TEA_STEEP_SK;
  teaTypes[2].steepTime = read_key_or_default(HERBAL_TEA_STEEP_SK, 4.50);

  teaTypes[3].name = "Oolong";
  teaTypes[3].temperature = "80-85ºC / 176-185ºF";
  teaTypes[3].storageKey = OOLONG_TEA_STEEP_SK;
  teaTypes[3].steepTime = read_key_or_default(OOLONG_TEA_STEEP_SK, 2.50);

  teaTypes[4].name = "White";
  teaTypes[4].temperature = "65-70ºC	/ 149-158ºF";
  teaTypes[4].storageKey = WHITE_TEA_STEEP_SK;
  teaTypes[4].steepTime = read_key_or_default(WHITE_TEA_STEEP_SK, 2.50);

  teaTypes[5].name = "Yellow";
  teaTypes[5].temperature = "70-75ºC	/ 158-167ºF";
  teaTypes[5].storageKey = YELLOW_TEA_STEEP_SK;
  teaTypes[5].steepTime = read_key_or_default(YELLOW_TEA_STEEP_SK, 1.50);


  main_window = window_create();
  window_stack_push(main_window, true);

  Layer* window_layer = window_get_root_layer(main_window);
  GRect bounds = layer_get_bounds(window_layer);

  menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  menu_layer_set_click_config_onto_window(menu_layer, main_window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

void handle_deinit(void) {
  menu_layer_destroy(menu_layer);
  window_destroy(main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
