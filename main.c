//not my originasl work.  a frankenstein piece from lots of projects.

#include <pebble.h>


static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  
  // Display this time on the Textlayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  
// Copy date into buffer from tm structure
static char date_buffer[16];
strftime(date_buffer, sizeof(date_buffer), "%a %b %d", tick_time);

// Show the date
text_layer_set_text(s_date_layer, date_buffer);
}

static void update_timer_callback(void *data) {
  // Make Time Invisible Clem's a damn genius that's who he is
  layer_set_hidden((Layer *)s_time_layer, true);
  layer_set_hidden((Layer *)s_date_layer, true);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {

  // A tap event occured
  update_time();
  // Make Time Visible
  
  
  // This is where Clem got his shit together
  layer_set_hidden((Layer *)s_time_layer, false);
  layer_set_hidden((Layer *)s_date_layer, false);
  
  text_layer_set_text_color(s_time_layer, PBL_IF_COLOR_ELSE(GColorBlack, GColorWhite));
  // Register an timer for 5 seconds to make the time invisible
  AppTimer *updateTimer = app_timer_register(3000, (AppTimerCallback) update_timer_callback, NULL);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  // Create GBitmap
s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

// Create BitmapLayer to display the GBitmap
s_background_layer = bitmap_layer_create(bounds);

// Set the bitmap onto the layer and add to the window
bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  
  // Create date TextLayer
s_date_layer = text_layer_create(GRect(0, 102, 144, 30));
text_layer_set_text_color(s_date_layer, GColorBlack);
text_layer_set_background_color(s_date_layer, GColorClear);
text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

// Add to Window
layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 115), bounds.size.w, 50));
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  
// Destroy GBitmap
gbitmap_destroy(s_background_bitmap);

// Destroy BitmapLayer
bitmap_layer_destroy(s_background_layer);

  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  // Destroy Date
  text_layer_destroy(s_date_layer);
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Subscribe to tap events
  accel_tap_service_subscribe(accel_tap_handler);
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Set Window background color
  window_set_background_color(s_main_window, GColorBlack);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}