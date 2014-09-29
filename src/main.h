
void update_time_layer(time_t time);
void up_single_click_handler(ClickRecognizerRef recognizer, void *context);
void down_single_click_handler(ClickRecognizerRef recognizer, void *context);
uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data);
uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
void timer_update_handler(struct tm *tick_time, TimeUnits units_changed);
void select_single_click_handler(ClickRecognizerRef recognizer, void *context);
void back_single_click_handler(ClickRecognizerRef recognizer, void *context);
void config_provider(Window *window);
void show_timer(const char *tea_name, const char *temperature_text, double steep_time);
void deallocate_timer_window();
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data);
void handle_init(void);
void handle_deinit(void);
int main(void);
