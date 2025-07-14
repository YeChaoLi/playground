/**
 * @file ui_screen_settings.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../ui.h"
#include "ui_screens.h"
#include "../components/ui_title_bar.h"
#include "../components/ui_switch.h"
#include "../components/ui_slider.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ui_screen_settings_loaded(lv_event_t * e);
static void ui_screen_settings_exit(lv_event_t * e);

static void ui_settings_close(lv_event_t * e);
static void ui_about_event(lv_event_t * e);
static void ui_stat_event(lv_event_t * e);
static void ui_dry_mode_event(lv_event_t * e);
static void ui_open_wifi_screen(lv_event_t * e);

static lv_obj_t * create_list_item(lv_obj_t * parent, const char * title);
static lv_obj_t * create_list_info(lv_obj_t * parent, const char * title, const char * info);
static lv_obj_t * create_list_item_switch(lv_obj_t * parent, const char * title, const char * info);
static lv_obj_t * create_slider_item(lv_obj_t * parent, const char *start, const char *end);
static lv_obj_t * create_text_item(lv_obj_t * parent, const char *text);


static void wifi_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void wifi_ssid_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void wifi_ip_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * settings_screen;
static lv_obj_t * logo_image;
static lv_obj_t * title_bar;
static lv_obj_t * list_panel;
static lv_obj_t * second_title;
static lv_obj_t * about_list;
static lv_obj_t * stats_list;
static lv_obj_t * about_btn;
static lv_obj_t * stat_btn;

static lv_obj_t * dry_mode;
static lv_obj_t * drying_btn;
static lv_obj_t * title_dry;
static lv_obj_t * wifi_btn;

static lv_obj_t * wifi_text;


static char device_model_buffer[OBSERVER_TEXT_BUFFER_SIZE];
static char device_serial_buffer[OBSERVER_TEXT_BUFFER_SIZE];
static char device_firmware_buffer[OBSERVER_TEXT_BUFFER_SIZE];


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_screen_settings_init(void)
{
	/* Device about subjects */
	lv_subject_init_string(&subject_device_model, device_model_buffer, NULL, OBSERVER_TEXT_BUFFER_SIZE, "Raymond");
	lv_subject_init_string(&subject_device_serial, device_serial_buffer, NULL, OBSERVER_TEXT_BUFFER_SIZE, "Cure2L-Awesome");
	lv_subject_init_string(&subject_device_firmware, device_firmware_buffer, NULL, OBSERVER_TEXT_BUFFER_SIZE, "1.2.3.2.1");

	/* Device usage statistics subjects */
	lv_subject_init_int(&subject_stats_cure_cycles, 0);
	lv_subject_init_int(&subject_stats_cure_time, 0);
	lv_subject_init_int(&subject_stats_cure_aborts, 0);
	lv_subject_init_int(&subject_heater_cycle_left, 0);
	lv_subject_init_int(&subject_heater_cycle_right, 0);
	lv_subject_init_int(&subject_heater_time_left, 0);
	lv_subject_init_int(&subject_heater_time_right, 0);
	lv_subject_init_int(&subject_led_usage_total, 0);


	settings_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(settings_screen, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(settings_screen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(settings_screen, LV_OBJ_FLAG_SCROLLABLE);

	title_bar = ui_title_bar_create(settings_screen, "Settings");

	second_title = ui_title_bar_create(settings_screen, "About this device");
	lv_obj_add_flag(second_title, LV_OBJ_FLAG_HIDDEN);
	lv_obj_set_style_bg_color(second_title, COLOR_BACKGROUND, 0);
	lv_obj_set_style_bg_opa(second_title, 255, 0);

	title_dry = ui_title_bar_create(settings_screen, "Drying mode");
	lv_obj_add_flag(title_dry, LV_OBJ_FLAG_HIDDEN);
	lv_obj_set_style_bg_color(title_dry, COLOR_BACKGROUND, 0);
	lv_obj_set_style_bg_opa(title_dry, 255, 0);

	lv_obj_t *toggle = ui_switch_create(title_dry);
	lv_obj_set_align(toggle, LV_ALIGN_RIGHT_MID);
	lv_obj_set_x(toggle, -40);
	lv_obj_add_state(toggle, LV_STATE_CHECKED);
	lv_button_bind_checked(toggle, &subject_drying_mode_switch);


	/* Main settings list */
	list_panel = lv_obj_create(settings_screen);
	lv_obj_remove_style_all(list_panel);
	lv_obj_set_width(list_panel, UI_SCREEN_WIDTH);
	lv_obj_set_height(list_panel, 300);
	lv_obj_set_x(list_panel, 0);
	lv_obj_set_y(list_panel, 100);
	lv_obj_set_flex_flow(list_panel,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(list_panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_scrollbar_mode(list_panel, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scroll_dir(list_panel, LV_DIR_VER);

	about_btn = create_list_item(list_panel, "About this device");

	wifi_btn = create_list_item_switch(list_panel, "Wi-Fi", "Lord of the pings: 192.168.1.8");
	wifi_text = lv_obj_get_child(lv_obj_get_child(wifi_btn, 1), 0);
	lv_button_bind_checked(lv_obj_get_child(lv_obj_get_child(wifi_btn, 1), 1), &subject_wifi_switch);

	drying_btn = create_list_item_switch(list_panel, "Drying mode", "5 min");
	lv_label_bind_text(lv_obj_get_child(lv_obj_get_child(drying_btn, 1), 0), &subject_drying_mode_value, "%d min");
	lv_button_bind_checked(lv_obj_get_child(lv_obj_get_child(drying_btn, 1), 1), &subject_drying_mode_switch);

	stat_btn = create_list_item(list_panel, "Usage statistics");

	/* Dry mode settings */
	dry_mode = lv_obj_create(settings_screen);
	lv_obj_remove_style_all(dry_mode);
	lv_obj_set_width(dry_mode, UI_SCREEN_WIDTH);
	lv_obj_set_height(dry_mode, 300);
	lv_obj_set_y(dry_mode, 100);
	lv_obj_set_flex_flow(dry_mode,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(dry_mode, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_scrollbar_mode(dry_mode, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scroll_dir(dry_mode, LV_DIR_VER);
	lv_obj_add_flag(dry_mode, LV_OBJ_FLAG_HIDDEN);
	lv_obj_set_style_bg_color(dry_mode, COLOR_BACKGROUND, 0);
	lv_obj_set_style_bg_opa(dry_mode, 255, 0);

	lv_obj_t *obj = create_list_info(dry_mode, "Time", "5 min");
	lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_label_bind_text(lv_obj_get_child(obj, 1), &subject_drying_mode_value, "%d min");
	obj = create_slider_item(dry_mode, "1 min", "15 min");
	lv_slider_bind_value(lv_obj_get_child(obj, 1), &subject_drying_mode_value);
	obj = create_text_item(dry_mode, "Recommended dry time is 3 minutes to ensure parts are fully dry before post-curing.");



	/* About settings list */
	about_list = lv_obj_create(settings_screen);
	lv_obj_remove_style_all(about_list);
	lv_obj_set_width(about_list, UI_SCREEN_WIDTH);
	lv_obj_set_height(about_list, 300);
	lv_obj_set_y(about_list, 100);
	lv_obj_set_flex_flow(about_list,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(about_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_scrollbar_mode(about_list, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scroll_dir(about_list, LV_DIR_VER);
	lv_obj_add_flag(about_list, LV_OBJ_FLAG_HIDDEN);
	lv_obj_set_style_bg_color(about_list, COLOR_BACKGROUND, 0);
	lv_obj_set_style_bg_opa(about_list, 255, 0);

	lv_obj_t * item;
	item = create_list_info(about_list, "Model", "Raymond");
	lv_label_bind_text(lv_obj_get_child(item, 1), &subject_device_model, NULL);
	item = create_list_info(about_list, "Serial name", "Cure2L-Awesome");
	lv_label_bind_text(lv_obj_get_child(item, 1), &subject_device_serial, NULL);
	item = create_list_info(about_list, "Firmware", "1.2.3.2.1");
	lv_label_bind_text(lv_obj_get_child(item, 1), &subject_device_firmware, NULL);

	/* Statistics settings list */
	stats_list = lv_obj_create(settings_screen);
	lv_obj_remove_style_all(stats_list);
	lv_obj_set_width(stats_list, UI_SCREEN_WIDTH);
	lv_obj_set_height(stats_list, 300);
	lv_obj_set_y(stats_list, 100);
	lv_obj_set_flex_flow(stats_list,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(stats_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_scrollbar_mode(stats_list, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scroll_dir(stats_list, LV_DIR_VER);
	lv_obj_add_flag(stats_list, LV_OBJ_FLAG_HIDDEN);
	lv_obj_set_style_bg_color(stats_list, COLOR_BACKGROUND, 0);
	lv_obj_set_style_bg_opa(stats_list, 255, 0);

	item = create_list_info(stats_list, "Cure cycles", "253");
	lv_label_bind_text(lv_obj_get_child(item, 1), &subject_stats_cure_cycles, "%d");

	item = create_list_info(stats_list, "Cure aborts", "35");
	lv_label_bind_text(lv_obj_get_child(item, 1), &subject_stats_cure_aborts, "%d");

	item = create_list_info(stats_list, "Cure time", "942 min");
	lv_label_bind_text(lv_obj_get_child(item, 1), &subject_stats_cure_time, "%d min");

	item = create_list_info(stats_list, "Left heater cycles", "94");
	lv_label_bind_text(lv_obj_get_child(item, 1), &subject_heater_cycle_left, "%d");
	item = create_list_info(stats_list, "Right heater cycles", "94");
	lv_label_bind_text(lv_obj_get_child(item, 1), &subject_heater_cycle_right, "%d");

	item = create_list_info(stats_list, "Left heater time", "123 min");
	lv_label_bind_text(lv_obj_get_child(item, 1), &subject_heater_time_left, "%d min");
	item = create_list_info(stats_list, "Right heater time", "123 min");
	lv_label_bind_text(lv_obj_get_child(item, 1), &subject_heater_time_right, "%d min");

	item = create_list_info(stats_list, "Total LED usage", "159");
	lv_label_bind_text(lv_obj_get_child(item, 1), &subject_led_usage_total, "%d min");

	lv_obj_add_event_cb(about_btn, ui_about_event, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(stat_btn, ui_stat_event, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(drying_btn, ui_dry_mode_event, LV_EVENT_CLICKED, NULL);

	lv_obj_add_event_cb(second_title, ui_settings_close, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(title_dry, ui_settings_close, LV_EVENT_CLICKED, NULL);

	lv_obj_add_event_cb(wifi_btn, ui_open_wifi_screen, LV_EVENT_CLICKED, NULL);

	lv_subject_add_observer(&subject_wifi_switch, wifi_switch_observer_cb, NULL);
	lv_subject_add_observer(&subject_wifi_ssid, wifi_ssid_observer_cb, NULL);
	lv_subject_add_observer(&subject_wifi_ip, wifi_ip_observer_cb, NULL);


	lv_obj_add_event_cb(title_bar, ui_screen_settings_exit, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(settings_screen, ui_screen_settings_loaded, LV_EVENT_SCREEN_LOADED, NULL);
}

lv_obj_t * ui_screen_settings(void)
{
	if (!settings_screen){
		ui_screen_settings_init();
	}
	return settings_screen;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/


static lv_obj_t * create_list_item(lv_obj_t * parent, const char * title)
{

	lv_obj_t *panel = lv_obj_create(parent);
	lv_obj_remove_style_all(panel);
	lv_obj_set_width(panel, UI_SCREEN_WIDTH);
	lv_obj_set_height(panel, 100);
	lv_obj_set_style_border_color(panel, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(panel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_TOP, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(panel, COLOR_INTERACTIVE_SECONDARY, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_style_bg_opa(panel, 255, LV_PART_MAIN| LV_STATE_PRESSED);
	lv_obj_set_style_pad_right(panel, 30, LV_PART_MAIN| LV_STATE_DEFAULT);


	LV_FONT_DECLARE(font_proxima_35);
	lv_obj_t *text = lv_label_create(panel);
	lv_obj_set_width(text, LV_SIZE_CONTENT);
	lv_obj_set_height(text, LV_SIZE_CONTENT); 
	lv_obj_set_x(text, 30);
	lv_obj_set_y(text, 0);
	lv_obj_set_align(text, LV_ALIGN_LEFT_MID);
	lv_label_set_text(text, title);
	lv_obj_set_style_text_font(text, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(text, COLOR_TEXT_PRIMARY, 0);

	LV_IMAGE_DECLARE(icon_chevron_right);

	lv_obj_t *icon = lv_image_create(panel);
	lv_image_set_src(icon, &icon_chevron_right);
	lv_obj_set_width(icon, LV_SIZE_CONTENT);
	lv_obj_set_height(icon, LV_SIZE_CONTENT); 
	lv_obj_set_align(icon, LV_ALIGN_RIGHT_MID);
	lv_obj_remove_flag(icon, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_image_recolor(icon, lv_color_hex(0xFFFFFF), LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_image_recolor_opa(icon, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

	return panel;
}

static lv_obj_t * create_list_info(lv_obj_t * parent, const char * title, const char * info)
{
	lv_obj_t *panel = lv_obj_create(parent);
	lv_obj_remove_style_all(panel);
	lv_obj_set_width(panel, UI_SCREEN_WIDTH);
	lv_obj_set_height(panel, 100);
	lv_obj_remove_flag(panel, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_set_style_border_color(panel, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(panel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_TOP, LV_PART_MAIN| LV_STATE_DEFAULT);

	LV_FONT_DECLARE(font_proxima_35);

	lv_obj_t *text = lv_label_create(panel);
	lv_obj_set_width(text, LV_SIZE_CONTENT);
	lv_obj_set_height(text, LV_SIZE_CONTENT); 
	lv_obj_set_x(text, 30);
	lv_obj_set_y(text, 0);
	lv_obj_set_align(text, LV_ALIGN_LEFT_MID);
	lv_label_set_text(text, title);
	lv_obj_set_style_text_font(text, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(text, COLOR_TEXT_PRIMARY, 0);

	lv_obj_t *text_info = lv_label_create(panel);
	lv_obj_set_width(text_info, LV_SIZE_CONTENT);
	lv_obj_set_height(text_info, LV_SIZE_CONTENT); 
	lv_obj_set_x(text_info, -30);
	lv_obj_set_y(text_info, 0);
	lv_obj_set_align(text_info, LV_ALIGN_RIGHT_MID);
	lv_label_set_text(text_info, info);
	lv_obj_set_style_text_align(text_info, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(text_info, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(text_info, COLOR_TEXT_SECONDARY, 0);

	return panel;
}

static lv_obj_t * create_list_item_switch(lv_obj_t * parent, const char * title, const char *info)
{

	lv_obj_t *panel = lv_obj_create(parent);
	lv_obj_remove_style_all(panel);
	lv_obj_set_width(panel, UI_SCREEN_WIDTH);
	lv_obj_set_height(panel, 100);
	lv_obj_set_style_border_color(panel, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(panel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_TOP, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(panel, COLOR_INTERACTIVE_SECONDARY, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_style_bg_opa(panel, 255, LV_PART_MAIN| LV_STATE_PRESSED);
	lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE );


	LV_FONT_DECLARE(font_proxima_35);
	lv_obj_t *text = lv_label_create(panel);
	lv_obj_set_width(text, LV_SIZE_CONTENT);
	lv_obj_set_height(text, LV_SIZE_CONTENT); 
	lv_obj_set_x(text, 30);
	lv_obj_set_y(text, 0);
	lv_obj_set_align(text, LV_ALIGN_LEFT_MID);
	lv_label_set_text(text, title);
	lv_obj_set_style_text_font(text, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(text, COLOR_TEXT_PRIMARY, 0);


	lv_obj_t *right_panel = lv_obj_create(panel);
	lv_obj_remove_style_all(right_panel);
	lv_obj_set_height(right_panel, 100);
	lv_obj_set_width(right_panel, LV_SIZE_CONTENT);
	lv_obj_set_align(right_panel, LV_ALIGN_RIGHT_MID );
	lv_obj_set_flex_flow(right_panel,LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(right_panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_remove_flag(right_panel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE ); 
	lv_obj_set_style_pad_left(right_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(right_panel, 30, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(right_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(right_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_column(right_panel, 30, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(right_panel, LV_OBJ_FLAG_SCROLLABLE );


	lv_obj_t *info_text = lv_label_create(right_panel);
	lv_obj_set_width(info_text, LV_SIZE_CONTENT);
	lv_obj_set_height(info_text, LV_SIZE_CONTENT); 
	lv_obj_set_x(info_text, -240);
	lv_obj_set_align(info_text, LV_ALIGN_RIGHT_MID);
	lv_label_set_text(info_text, info);
	lv_obj_set_style_text_font(info_text, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(info_text, COLOR_TEXT_SECONDARY, 0);

	lv_obj_t *toggle = ui_switch_create(right_panel);
	lv_obj_set_align(toggle, LV_ALIGN_RIGHT_MID);
	lv_obj_set_x(toggle, -120);
	lv_obj_add_state(toggle, LV_STATE_CHECKED);
	// lv_obj_add_event_cb(toggle, ui_switch_clicked_event, LV_EVENT_CLICKED, NULL);

	LV_IMAGE_DECLARE(icon_chevron_right);

	lv_obj_t *icon = lv_image_create(right_panel);
	lv_image_set_src(icon, &icon_chevron_right);
	lv_obj_set_width(icon, 40);
	lv_obj_set_height(icon, LV_SIZE_CONTENT); 
	lv_obj_set_align(icon, LV_ALIGN_RIGHT_MID);
	lv_obj_remove_flag(icon, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_image_recolor(icon, lv_color_hex(0xFFFFFF), LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_image_recolor_opa(icon, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

	return panel;
}

static lv_obj_t * create_slider_item(lv_obj_t * parent, const char *start, const char *end)
{
	lv_obj_t *panel = lv_obj_create(parent);
	lv_obj_remove_style_all(panel);
	lv_obj_set_width(panel, UI_SCREEN_WIDTH);
	lv_obj_set_height(panel, 100);
	lv_obj_remove_flag(panel, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_set_style_border_color(panel, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(panel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_TOP, LV_PART_MAIN| LV_STATE_DEFAULT);

	LV_FONT_DECLARE(font_proxima_35);

	lv_obj_t *text_start = lv_label_create(panel);
	lv_obj_set_width(text_start, LV_SIZE_CONTENT);
	lv_obj_set_height(text_start, LV_SIZE_CONTENT); 
	lv_obj_set_x(text_start, 30);
	lv_obj_set_y(text_start, 0);
	lv_obj_set_align(text_start, LV_ALIGN_LEFT_MID);
	lv_label_set_text(text_start, start);
	lv_obj_set_style_text_font(text_start, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(text_start, COLOR_TEXT_PRIMARY, 0);

	lv_obj_t *slider = ui_slider_create(panel);
	lv_obj_set_width(slider, 980);
	lv_obj_set_align(slider, LV_ALIGN_CENTER);
	lv_obj_set_x(slider, -5);
	lv_slider_set_range(slider, 1, 15);
	lv_slider_set_value(slider, 7, LV_ANIM_OFF);

	lv_obj_t *text_end = lv_label_create(panel);
	lv_obj_set_width(text_end, LV_SIZE_CONTENT);
	lv_obj_set_height(text_end, LV_SIZE_CONTENT); 
	lv_obj_set_x(text_end, -30);
	lv_obj_set_y(text_end, 0);
	lv_obj_set_align(text_end, LV_ALIGN_RIGHT_MID);
	lv_label_set_text(text_end, end);
	lv_obj_set_style_text_align(text_end, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(text_end, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(text_end, COLOR_TEXT_PRIMARY, 0);

	return panel;

}

static lv_obj_t * create_text_item(lv_obj_t * parent, const char *text)
{
	lv_obj_t *panel = lv_obj_create(parent);
	lv_obj_remove_style_all(panel);
	lv_obj_set_width(panel, UI_SCREEN_WIDTH);
	lv_obj_set_height(panel, LV_SIZE_CONTENT);
	lv_obj_remove_flag(panel, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_set_style_border_color(panel, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(panel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_NONE, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(panel, 5, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(panel, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(panel, 30, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(panel, 30, LV_PART_MAIN| LV_STATE_DEFAULT);

	LV_FONT_DECLARE(font_proxima_28);

	lv_obj_t *text_start = lv_label_create(panel);
	lv_obj_set_width(text_start, lv_pct(100));
	lv_obj_set_height(text_start, LV_SIZE_CONTENT); 
	lv_obj_set_align(text_start, LV_ALIGN_TOP_LEFT);
	lv_label_set_text(text_start, text);
	lv_obj_set_style_text_font(text_start, &font_proxima_28, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(text_start, COLOR_TEXT_SECONDARY, 0);

	return panel;

}

static void ui_screen_settings_exit(lv_event_t * e) {
    lv_screen_load(ui_screen_home());
}

static void ui_open_wifi_screen(lv_event_t * e)
{
	lv_screen_load(ui_screen_wifi());
}

static void ui_screen_settings_loaded(lv_event_t * e) {
    on_screen_loaded_cb(SCREEN_SETTINGS);
}

static void ui_about_event(lv_event_t * e)
{
	lv_label_set_text(lv_obj_get_child(second_title, 1), "About this device");
	

	lv_obj_remove_flag(about_list, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(second_title, LV_OBJ_FLAG_HIDDEN);

}
static void ui_stat_event(lv_event_t * e)
{

	lv_label_set_text(lv_obj_get_child(second_title, 1), "Usage statistics");
	

	lv_obj_remove_flag(stats_list, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(second_title, LV_OBJ_FLAG_HIDDEN);
	
}

static void ui_dry_mode_event(lv_event_t * e)
{


	lv_obj_remove_flag(dry_mode, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(title_dry, LV_OBJ_FLAG_HIDDEN);
	
}


static void ui_settings_close(lv_event_t * e)
{
	lv_obj_add_flag(about_list, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(stats_list, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(dry_mode, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(second_title, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(title_dry, LV_OBJ_FLAG_HIDDEN);
}

static void wifi_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	int32_t state = lv_subject_get_int(subject);
	if (!state) {
		lv_subject_copy_string(&subject_wifi_ssid, "");
	}
}

static void wifi_ssid_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	const char *ssid = lv_subject_get_string(subject);
	const char *ip = lv_subject_get_string(&subject_wifi_ip);
	
	if (!ssid || ssid[0] == '\0') {
		lv_label_set_text(wifi_text, "");
	} else {
		lv_label_set_text_fmt(wifi_text, "%s: %s", ssid, ip);
	}

}

static void wifi_ip_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	const char *ssid = lv_subject_get_string(&subject_wifi_ssid);
	const char *ip = lv_subject_get_string(subject);

	if (!ssid || ssid[0] == '\0') {
		lv_label_set_text(wifi_text, "");
	} else {
		lv_label_set_text_fmt(wifi_text, "%s: %s", ssid, ip);
	}
}

