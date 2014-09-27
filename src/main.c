#include <pebble.h>

#define MENU_SECTIONS 1
#define MENU_ITEMS 5
  
Window *main_window;

MenuLayer *menu_layer;
  
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return MENU_SECTIONS;
}


static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_ITEMS;
}


static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}


static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  return menu_cell_basic_header_draw(ctx, cell_layer, "Select tea type");
}


static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  
  switch(cell_index->row) {
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, "Black tea", "100ºC / 210ºF", NULL);
      break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "Green tea", "75-80ºC / 165-175ºF", NULL);
      break;
    case 2:
      menu_cell_basic_draw(ctx, cell_layer, "Herbal tea", "100ºC / 210ºF", NULL);  
      break;
    case 3: 
      menu_cell_basic_draw(ctx, cell_layer, "Oolong tea", "80-85ºC / 175-185ºF", NULL);
      break;
    case 4: 
      menu_cell_basic_draw(ctx, cell_layer, "White tea", "65-70ºC	/ 150-155ºF", NULL);  
      break;
  }
}

char* floatToString(char* buffer, int bufferSize, double number)
{
	snprintf(buffer, bufferSize, "%d", (int)number);
	return buffer;
}

void show_timer(const char *tea_name, const char *temperature_text, double steep_time) {
  
  Window *timer_window = window_create();
  Layer *window_layer = window_get_root_layer(timer_window);
  GRect bounds = layer_get_bounds(window_layer);
  
  Layer *timer_layer = layer_create(bounds);
  
  TextLayer *tea_name_layer = text_layer_create(GRect(0, 0, bounds.size.w, 35));
  text_layer_set_text(tea_name_layer, tea_name);
  text_layer_set_size(tea_name_layer, GSize(bounds.size.w, 35));
  text_layer_set_text_alignment(tea_name_layer, GTextAlignmentCenter);
  text_layer_set_font(tea_name_layer,fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  
  TextLayer *temperature_layer = text_layer_create(GRect(0, 36, bounds.size.w, 18));
  text_layer_set_text(temperature_layer, temperature_text);
  text_layer_set_size(temperature_layer, GSize(bounds.size.w, 18));
  text_layer_set_text_alignment(temperature_layer, GTextAlignmentCenter);
  text_layer_set_font(temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  
  TextLayer *start_help_text = text_layer_create(GRect(0, bounds.size.h - 16, bounds.size.w, 16));
  text_layer_set_text(start_help_text, "Middle button to start");
  text_layer_set_size(start_help_text, GSize(bounds.size.w, 16));
  text_layer_set_text_alignment(start_help_text, GTextAlignmentCenter);
  text_layer_set_font(start_help_text, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  
  char minutes_text[5], seconds_text[3];
  char* minutes = floatToString(minutes_text, 5, (int)(steep_time * 60) / 60);
  char* seconds = floatToString(seconds_text, 3, (int)(steep_time * 60) % 60);
  TextLayer *count_down_layer = text_layer_create(GRect(0, 58, bounds.size.w, 45));
  text_layer_set_text(count_down_layer, strcat(strcat(minutes, ":"), seconds));
  text_layer_set_size(count_down_layer, GSize(bounds.size.w, 45));
  text_layer_set_text_alignment(count_down_layer, GTextAlignmentCenter);
  text_layer_set_font(count_down_layer,fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  
  
  layer_add_child(timer_layer, text_layer_get_layer(tea_name_layer));
  layer_add_child(timer_layer, text_layer_get_layer(temperature_layer));
  layer_add_child(timer_layer, text_layer_get_layer(count_down_layer));
  layer_add_child(timer_layer, text_layer_get_layer(start_help_text));
  layer_add_child(window_layer, timer_layer);
  
  window_stack_push(timer_window, true);
  
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
 
  switch(cell_index->row) {
    case 0:
      show_timer("Black", "100ºC / 210ºF", 2.50);
      break;
    case 1:
      show_timer("Green", "75-80ºC / 165-175ºF", 2.50);
      break;
    case 2:
      show_timer("Herbal", "100ºC / 210ºF", 2.50);
      break;
    case 3:
      show_timer("Oolong", "80-85ºC / 175-185ºF", 2.50);
      break;
    case 4:
      show_timer("White", "65-70ºC	/ 150-155ºF", 2.50);
      break;
  }
}

void handle_init(void) {
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
  window_destroy(main_window); 
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
