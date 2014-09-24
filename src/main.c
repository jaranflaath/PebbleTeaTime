#include <pebble.h>

Window *main_window;
Window *teatime_window;
TextLayer *text_layer;
BitmapLayer *splashLayer;
GBitmap *splashImage;
  
void handle_init(void) {
  main_window = window_create();
  teatime_window = window_create();
  window_stack_push(main_window, true);
  
  splashImage = gbitmap_create_with_resource(RESOURCE_ID_TEACUP_BW);

  Layer* window_layer = window_get_root_layer(main_window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // The bitmap layer holds the image for display
  splashLayer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(splashLayer, splashImage);
  bitmap_layer_set_alignment(splashLayer, GAlignCenter);
  
  Layer* bitmapLayer = bitmap_layer_get_layer(splashLayer);
  layer_add_child(window_layer, bitmapLayer);
  /*psleep(1000);
  window_stack_pop(true);
  
  text_layer = text_layer_create(GRect(0, 0, 144, 20));
  text_layer_set_text(text_layer, "Select type of tea");
  
  layer_add_child(window_get_root_layer(teatime_window), text_layer_get_layer(text_layer));
  
  window_stack_push(teatime_window, true);*/
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  gbitmap_destroy(splashImage);
  window_destroy(main_window);
  window_destroy(teatime_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
