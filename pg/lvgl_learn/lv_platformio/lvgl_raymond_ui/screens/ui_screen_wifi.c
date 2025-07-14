/**
 * @file ui_screen_wifi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../ui.h"
#include "ui_screens.h"
#include "../components/ui_title_bar.h"
#include "../components/ui_switch.h"
#include "../components/ui_button.h"
#include "../components/ui_keyboard.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ui_screen_wifi_loaded(lv_event_t *e);
static void ui_screen_wifi_exit(lv_event_t * e);

static void ui_open_wifi_details(lv_event_t * e);
static void ui_close_wifi_details(lv_event_t * e);

static void ui_password_toggle(lv_event_t * e);
static void ui_password_input(lv_event_t * e);
static void ui_forget_wifi(lv_event_t * e);

static void ui_textarea_event_cb(lv_event_t *e);

static lv_obj_t * create_list_item_switch(lv_obj_t * parent, const char * title);
static lv_obj_t * create_list_info(lv_obj_t * parent, const char * title, const char * info);
static lv_obj_t * create_wifi_list_item(lv_obj_t * parent, wifi_detail_t *wifi);

static void wifi_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void wifi_anim_exec_cb(lv_anim_t* a, int32_t v);
static void wifi_anim_complete_cb(lv_anim_t *a);
static void wifi_anim_start();

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *wifi_screen;
static lv_obj_t *wifi_info;
static lv_obj_t *wifi_info_list;
static lv_obj_t *wifi_panel_list;
static lv_obj_t *wifi_status_text;
static lv_obj_t *wifi_password_panel;
static lv_obj_t *keyboard;
static lv_obj_t *password_textarea;

static lv_obj_t *wifi_status_label;
static lv_obj_t *wifi_strength_label;
static lv_obj_t *wifi_ip_label;


static wifi_detail_t wifi_list[] = {
	{"Test Secure (Not connected)", -50, true},
	{"Test Secure (Connected)", -70, true},
	{"Test Secure (Password Error)", -60, true},
	{"Test Secure (No Internet)", -80, true},
	{"Test Secure (Disabled)", -90, true},
	{"Test Non-Secure (Not connected)", -50, false},
	{"Test Non-Secure (Connected)", -70, false},
	{"Test Secure (Password Error)", -60, true},
	{"Test Non-Secure (No Internet)", -80, false},
	{"Test Non-Secure (Disabled)", -90, false},
};

static lv_obj_t *prev_wifi_item;
static wifi_detail_t *prev_wifi_detail;

static lv_obj_t *selected_wifi_item;
static wifi_detail_t *selected_wifi_detail;

static lv_anim_t wifi_anim;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_screen_wifi_init(void)
{
	wifi_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(wifi_screen, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(wifi_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_remove_flag(wifi_screen, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t *title_bar = ui_title_bar_create(wifi_screen, "Wi-Fi");

	lv_obj_t *toggle = ui_switch_create(title_bar);
	lv_obj_set_align(toggle, LV_ALIGN_RIGHT_MID);
	lv_obj_set_x(toggle, -40);
	lv_obj_add_state(toggle, LV_STATE_CHECKED);
	lv_button_bind_checked(toggle, &subject_wifi_switch);

	wifi_panel_list = lv_obj_create(wifi_screen);
	lv_obj_remove_style_all(wifi_panel_list);
	lv_obj_set_width(wifi_panel_list, UI_SCREEN_WIDTH);
	lv_obj_set_height(wifi_panel_list, 300);
	lv_obj_set_x(wifi_panel_list, 0);
	lv_obj_set_y(wifi_panel_list, 100);
	lv_obj_set_flex_flow(wifi_panel_list,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(wifi_panel_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_scrollbar_mode(wifi_panel_list, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scroll_dir(wifi_panel_list, LV_DIR_VER);
	

	lv_obj_t *item;
	for (int i = 0; i < 10; i++) {
		item = create_wifi_list_item(wifi_panel_list, &wifi_list[i]);
	}

	LV_FONT_DECLARE(font_proxima_35);
	wifi_status_text = lv_label_create(wifi_screen);
	lv_obj_set_width(wifi_status_text, LV_SIZE_CONTENT);
	lv_obj_set_height(wifi_status_text, LV_SIZE_CONTENT); 
	lv_obj_set_align(wifi_status_text, LV_ALIGN_CENTER);
	lv_label_set_text(wifi_status_text, "No Wi-Fi Networks found.");
	lv_obj_set_style_text_font(wifi_status_text, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(wifi_status_text, COLOR_TEXT_SECONDARY, 0);


	/* Wifi info */
	wifi_info = lv_obj_create(wifi_screen);
	lv_obj_remove_style_all(wifi_info);
	lv_obj_set_width(wifi_info, UI_SCREEN_WIDTH);
	lv_obj_set_height(wifi_info, UI_SCREEN_HEIGHT);
	lv_obj_set_flex_flow(wifi_info,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(wifi_info, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_scrollbar_mode(wifi_info, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scroll_dir(wifi_info, LV_DIR_VER);
	lv_obj_set_style_bg_color(wifi_info, COLOR_BACKGROUND, 0);
	lv_obj_set_style_bg_opa(wifi_info, 255, 0);
	lv_obj_add_flag(wifi_info, LV_OBJ_FLAG_HIDDEN);

	lv_obj_t *info_title = ui_title_bar_create(wifi_info, "Wi-Fi details");
	lv_obj_add_event_cb(info_title, ui_close_wifi_details, LV_EVENT_CLICKED, NULL);

	wifi_info_list = lv_obj_create(wifi_info);
	lv_obj_remove_style_all(wifi_info_list);
	lv_obj_set_width(wifi_info_list, UI_SCREEN_WIDTH);
	lv_obj_set_height(wifi_info_list, 300);
	lv_obj_set_y(wifi_info_list, 100);
	lv_obj_set_flex_flow(wifi_info_list,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(wifi_info_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_scrollbar_mode(wifi_info_list, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scroll_dir(wifi_info_list, LV_DIR_VER);
	lv_obj_set_style_pad_bottom(wifi_info_list, 30, 0);

	item = create_list_info(wifi_info_list, "Status", "Connected");
	wifi_status_label = lv_obj_get_child(item, 1);
	item = create_list_info(wifi_info_list, "Signal Strength", "Excellent");
	wifi_strength_label = lv_obj_get_child(item, 1);
	item = create_list_info(wifi_info_list, "IP sddress", "192.168.1.8");
	wifi_ip_label = lv_obj_get_child(item, 1);
	lv_label_set_text(wifi_ip_label, "");
	lv_label_bind_text(wifi_ip_label, &subject_wifi_ip, NULL);

	lv_obj_t *btn_panel = lv_obj_create(wifi_info_list);
	lv_obj_remove_style_all(btn_panel);
	lv_obj_set_width(btn_panel, UI_SCREEN_WIDTH);
	lv_obj_set_height(btn_panel, 100);
	lv_obj_set_style_pad_right(btn_panel, 30, 0);

	lv_obj_t *btn = ui_button_destructive_create(btn_panel, "Forget Network");
	lv_obj_set_width(btn, LV_SIZE_CONTENT);
	lv_obj_set_align(btn, LV_ALIGN_RIGHT_MID);
	lv_obj_set_style_pad_left(lv_obj_get_child(btn, 0), 25, 0);
	lv_obj_set_style_pad_right(lv_obj_get_child(btn, 0), 25, 0);
	lv_obj_add_event_cb(btn, ui_forget_wifi, LV_EVENT_CLICKED, NULL);
	


	wifi_password_panel = lv_obj_create(wifi_screen);
	lv_obj_remove_style_all(wifi_password_panel);
	lv_obj_set_width(wifi_password_panel, UI_SCREEN_WIDTH);
	lv_obj_set_height(wifi_password_panel, UI_SCREEN_HEIGHT);
	lv_obj_set_style_bg_color(wifi_password_panel, COLOR_BACKGROUND, 0);
	lv_obj_set_style_bg_opa(wifi_password_panel, 255, 0);
	lv_obj_add_flag(wifi_password_panel, LV_OBJ_FLAG_HIDDEN);

	lv_obj_t *ui_passwordcont = lv_obj_create(wifi_password_panel);
	lv_obj_remove_style_all(ui_passwordcont);
	lv_obj_set_width(ui_passwordcont, 1280);
	lv_obj_set_height(ui_passwordcont, 100);
	lv_obj_set_align(ui_passwordcont, LV_ALIGN_TOP_MID);
	lv_obj_set_flex_flow(ui_passwordcont,LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(ui_passwordcont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_remove_flag(ui_passwordcont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);  
	lv_obj_set_style_border_color(ui_passwordcont, lv_color_hex(0x4B4B4B), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui_passwordcont, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui_passwordcont, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui_passwordcont, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN| LV_STATE_DEFAULT);

	LV_IMAGE_DECLARE(image_button_back);
	lv_obj_t *password_back = lv_image_create(ui_passwordcont);
	lv_image_set_src(password_back, &image_button_back);
	lv_obj_set_width(password_back, LV_SIZE_CONTENT);
	lv_obj_set_height(password_back, LV_SIZE_CONTENT);
	lv_obj_add_flag(password_back, LV_OBJ_FLAG_CLICKABLE); 
	lv_obj_remove_flag(password_back, LV_OBJ_FLAG_SCROLLABLE);  

	password_textarea = lv_textarea_create(ui_passwordcont);
	lv_obj_set_width(password_textarea, 1150);
	lv_obj_set_height(password_textarea, LV_SIZE_CONTENT);
	lv_obj_set_x(password_textarea, 80);
	lv_obj_set_y(password_textarea, 16);
	lv_textarea_set_placeholder_text(password_textarea,"Password");
	lv_textarea_set_one_line(password_textarea,true);
	lv_textarea_set_password_mode(password_textarea, true);
	lv_obj_set_style_text_color(password_textarea, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(password_textarea, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(password_textarea, 1, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(password_textarea, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(password_textarea, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(password_textarea, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(password_textarea, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(password_textarea, lv_color_hex(0x1967D2), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(password_textarea, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(password_textarea, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(password_textarea, 30, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(password_textarea, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(password_textarea, 15, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(password_textarea, 14, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(password_textarea, lv_color_hex(0xD21919), LV_PART_MAIN | LV_STATE_CHECKED);
	lv_obj_set_style_border_opa(password_textarea, 255, LV_PART_MAIN| LV_STATE_CHECKED);

	lv_obj_set_style_text_color(password_textarea, lv_color_hex(0x1976D2), LV_PART_CURSOR | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(password_textarea, 255, LV_PART_CURSOR| LV_STATE_DEFAULT);

	lv_obj_set_style_text_color(password_textarea, lv_color_hex(0x616161), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(password_textarea, 255, LV_PART_TEXTAREA_PLACEHOLDER| LV_STATE_DEFAULT);

	lv_obj_t * label = lv_textarea_get_label(password_textarea);
	lv_obj_set_y(label, 5); /* Ajdust offset for the text */

	lv_obj_add_event_cb(password_textarea, ui_textarea_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(password_textarea, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

	LV_IMAGE_DECLARE(icon_password_show);
	LV_IMAGE_DECLARE(icon_password_hide);
	lv_obj_t *password_toggle = lv_obj_create(password_textarea);
	lv_obj_remove_style_all(password_toggle);
	lv_obj_set_width(password_toggle, 80);
	lv_obj_set_height(password_toggle, 40);
	lv_obj_set_align(password_toggle, LV_ALIGN_RIGHT_MID);
	lv_obj_add_flag(password_toggle, LV_OBJ_FLAG_CHECKABLE); 
	lv_obj_remove_flag(password_toggle, LV_OBJ_FLAG_SCROLLABLE);  
	lv_obj_set_style_bg_image_src(password_toggle, &icon_password_show, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_image_src(password_toggle, &icon_password_hide, LV_PART_MAIN | LV_STATE_CHECKED);
	lv_obj_add_state(password_toggle, LV_STATE_CHECKED);
	lv_obj_add_event_cb(password_toggle, ui_password_toggle, LV_EVENT_VALUE_CHANGED, password_textarea);

	keyboard = ui_keyboard_create(wifi_password_panel);

	lv_keyboard_set_textarea(keyboard, password_textarea);

	lv_obj_add_event_cb(keyboard, ui_password_input, LV_EVENT_READY, NULL);

	lv_obj_add_event_cb(password_back, ui_close_wifi_details, LV_EVENT_CLICKED, NULL);


	lv_subject_add_observer(&subject_wifi_switch, wifi_switch_observer_cb, NULL);

	lv_obj_add_event_cb(title_bar, ui_screen_wifi_exit, LV_EVENT_CLICKED, NULL);

	lv_obj_add_event_cb(wifi_screen, ui_screen_wifi_loaded, LV_EVENT_SCREEN_LOADED, NULL);
}

lv_obj_t *ui_screen_wifi(void)
{
	if (!wifi_screen)
	{
		ui_screen_wifi_init();
	}
	return wifi_screen;
}

void wifi_clear_list(void)
{
	lv_obj_clean(wifi_panel_list);
	lv_obj_add_flag(wifi_panel_list, LV_OBJ_FLAG_HIDDEN);
	lv_label_set_text(wifi_status_text, "No Wi-Fi Networks found.");
}

void wifi_add_list(wifi_detail_t *wifi)
{
	create_wifi_list_item(wifi_panel_list, wifi);
}

void wifi_update_status(wifi_detail_t *wifi, uint8_t status)
{
	if (prev_wifi_item != NULL && prev_wifi_item != selected_wifi_item)
	{
		/* Invalidate the previous connected wifi */
		int32_t prev_index = lv_obj_get_index(prev_wifi_item);
		lv_obj_delete(prev_wifi_item);
		prev_wifi_detail->connected = false;
		lv_obj_t *prev = create_wifi_list_item(wifi_panel_list, prev_wifi_detail);
		lv_obj_move_to_index(prev, prev_index);
	}

	int32_t index = 0;
	if (selected_wifi_item != NULL)
	{
		index = lv_obj_get_index(selected_wifi_item);
		lv_obj_delete(selected_wifi_item);
	}
	selected_wifi_item = create_wifi_list_item(wifi_panel_list, wifi);
	lv_obj_move_to_index(selected_wifi_item, index);

	prev_wifi_item = selected_wifi_item;
	prev_wifi_detail = wifi;

	if (status == 1){
		lv_label_set_text(wifi_status_label, "Connected");

		char text[64];
		snprintf(text, sizeof(text), "%s", wifi->ip);
		lv_subject_copy_string(&subject_wifi_ip, text);

		snprintf(text, sizeof(text), "%s", wifi->ssid);
		lv_subject_copy_string(&subject_wifi_ssid, text);

	} else {
		lv_label_set_text(wifi_status_label, "Not connected");
		lv_subject_copy_string(&subject_wifi_ip, "");
		lv_subject_copy_string(&subject_wifi_ssid, "");
	}


	LV_IMAGE_DECLARE(icon_wifi_no_internet);
	LV_IMAGE_DECLARE(icon_wifi_disabled);
	switch (status)
	{
	case 0:
		/* Not connected */
		break;
	case 1:
		/* Connected */
		if (wifi->rssi >= -50){
			lv_label_set_text(wifi_strength_label, "Excellent");
		} else if (wifi->rssi >= -70){
			lv_label_set_text(wifi_strength_label, "Good");
		} else if (wifi->rssi >= -85){
			lv_label_set_text(wifi_strength_label, "Fair");
		} else {
			lv_label_set_text(wifi_strength_label, "Poor");
		}
		break;
	case 2:
		/* Password error */
		lv_obj_add_state(password_textarea, LV_STATE_CHECKED);
		lv_obj_remove_flag(wifi_password_panel, LV_OBJ_FLAG_HIDDEN);
		break;
	case 3:
		/* No internet */
		lv_image_set_src(lv_obj_get_child(lv_obj_get_child(selected_wifi_item, 0), 0), &icon_wifi_no_internet);
		break;
	case 4:
		/* WiFi Disabled */
		lv_image_set_src(lv_obj_get_child(lv_obj_get_child(selected_wifi_item, 0), 0), &icon_wifi_disabled);
	default:
		break;
	}
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ui_screen_wifi_loaded(lv_event_t *e)
{
	on_screen_loaded_cb(SCREEN_WIFI);
}

static void ui_screen_wifi_exit(lv_event_t *e)
{
	lv_screen_load(ui_screen_settings());
}

static void ui_open_wifi_details(lv_event_t * e)
{
	selected_wifi_detail = (wifi_detail_t *)lv_event_get_user_data(e);
	if (selected_wifi_item != NULL)
	{
		lv_obj_add_flag(lv_obj_get_child(lv_obj_get_child(selected_wifi_item, 1), 1), LV_OBJ_FLAG_HIDDEN);
	}
	selected_wifi_item = lv_event_get_target(e);
	if (selected_wifi_detail->connected)
	{
		lv_obj_remove_flag(wifi_info, LV_OBJ_FLAG_HIDDEN);
	} else 
	{
		if (selected_wifi_detail->secure)
		{
			lv_obj_remove_state(password_textarea, LV_STATE_CHECKED);
			lv_textarea_set_text(password_textarea, "");
			lv_buttonmatrix_set_selected_button(keyboard, LV_BUTTONMATRIX_BUTTON_NONE); /* Clear last pressed key */
			lv_obj_send_event(keyboard, LV_EVENT_VALUE_CHANGED, NULL); /* Send event to keyboard (this is to disable the Okay key) */
			lv_obj_remove_flag(wifi_password_panel, LV_OBJ_FLAG_HIDDEN);
		} else {
			wifi_selected_cb(selected_wifi_detail);
			lv_obj_remove_flag(lv_obj_get_child(lv_obj_get_child(selected_wifi_item, 1), 1), LV_OBJ_FLAG_HIDDEN);
		}
	}
	
}

static void ui_forget_wifi(lv_event_t * e)
{

	selected_wifi_detail->connected = false;
	wifi_forget_cb(selected_wifi_detail);
	wifi_update_status(selected_wifi_detail, 0);
	lv_obj_add_flag(wifi_info, LV_OBJ_FLAG_HIDDEN);

}

static void ui_password_input(lv_event_t * e)
{
	selected_wifi_detail->password = lv_textarea_get_text(password_textarea);
	wifi_selected_cb(selected_wifi_detail);
	lv_obj_add_flag(wifi_password_panel, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(lv_obj_get_child(lv_obj_get_child(selected_wifi_item, 1), 1), LV_OBJ_FLAG_HIDDEN);
}

static void ui_close_wifi_details(lv_event_t * e)
{
	lv_obj_add_flag(wifi_info, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(wifi_password_panel, LV_OBJ_FLAG_HIDDEN);
}

static void ui_password_toggle(lv_event_t * e)
{
	lv_obj_t * textarea = lv_event_get_user_data(e);
	lv_obj_t * toggle = lv_event_get_target(e);

	lv_textarea_set_password_mode(textarea, lv_obj_has_state(toggle, LV_STATE_CHECKED));
}

static void wifi_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	if (lv_subject_get_int(subject))
	{
		lv_obj_remove_flag(wifi_panel_list, LV_OBJ_FLAG_HIDDEN);
		lv_label_set_text(wifi_status_text, "");
	} else {
		lv_obj_add_flag(wifi_panel_list, LV_OBJ_FLAG_HIDDEN);
		lv_label_set_text(wifi_status_text, "Wi-Fi turned off.");
	}
}


static lv_obj_t * create_list_item_switch(lv_obj_t * parent, const char * title)
{

	lv_obj_t *panel = lv_obj_create(parent);
	lv_obj_remove_style_all(panel);
	lv_obj_set_width(panel, UI_SCREEN_WIDTH);
	lv_obj_set_height(panel, 100);
	lv_obj_set_style_border_color(panel, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(panel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_TOP, LV_PART_MAIN| LV_STATE_DEFAULT);
	// lv_obj_set_style_bg_color(panel, COLOR_INTERACTIVE_SECONDARY, LV_PART_MAIN | LV_STATE_PRESSED);
	// lv_obj_set_style_bg_opa(panel, 255, LV_PART_MAIN| LV_STATE_PRESSED);


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


	lv_obj_t *toggle = ui_switch_create(panel);
	lv_obj_set_align(toggle, LV_ALIGN_RIGHT_MID);
	lv_obj_set_x(toggle, -40);
	lv_obj_add_state(toggle, LV_STATE_CHECKED);
	// lv_obj_add_event_cb(toggle, ui_switch_clicked_event, LV_EVENT_CLICKED, NULL);


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

static lv_obj_t * create_wifi_list_item(lv_obj_t * parent, wifi_detail_t *wifi)
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
	lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE);


	lv_obj_t *left_panel = lv_obj_create(panel);
	lv_obj_remove_style_all(left_panel);
	lv_obj_set_height(left_panel, 100);
	lv_obj_set_width(left_panel, LV_SIZE_CONTENT);
	lv_obj_set_align(left_panel, LV_ALIGN_LEFT_MID);
	lv_obj_set_flex_flow(left_panel,LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(left_panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_remove_flag(left_panel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_left(left_panel, 30, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(left_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(left_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(left_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_row(left_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_column(left_panel, 30, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(left_panel, LV_OBJ_FLAG_SCROLLABLE);

	LV_IMAGE_DECLARE(icon_wifi_strong_signal);
	LV_IMAGE_DECLARE(icon_wifi_medium_signal);
	LV_IMAGE_DECLARE(icon_wifi_weak_signal);
	LV_IMAGE_DECLARE(icon_wifi_no_signal);
	lv_obj_t *signal_icon = lv_image_create(left_panel);
	if (wifi->rssi > -50){
		lv_image_set_src(signal_icon, &icon_wifi_strong_signal);
	}
	else if (wifi->rssi > -70){
		lv_image_set_src(signal_icon, &icon_wifi_medium_signal);
	}
	else if (wifi->rssi > -90){
		lv_image_set_src(signal_icon, &icon_wifi_weak_signal);
	}
	else {
		lv_image_set_src(signal_icon, &icon_wifi_no_signal);
	}
	lv_obj_set_width(signal_icon, 40);
	lv_obj_set_height(signal_icon, LV_SIZE_CONTENT);
	lv_obj_set_align(signal_icon, LV_ALIGN_CENTER);
	lv_obj_add_flag(signal_icon, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_remove_flag(signal_icon, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_image_recolor(signal_icon, lv_color_hex(0x1976D2), LV_PART_MAIN| LV_STATE_CHECKED);
	lv_obj_set_style_image_recolor_opa(signal_icon, 255, LV_PART_MAIN| LV_STATE_CHECKED);

	LV_FONT_DECLARE(font_proxima_35);
	lv_obj_t *text = lv_label_create(left_panel);
	lv_obj_set_width(text, LV_SIZE_CONTENT);
	lv_obj_set_height(text, LV_SIZE_CONTENT); 
	lv_label_set_text(text, wifi->ssid);
	lv_obj_set_style_text_font(text, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(text, COLOR_TEXT_PRIMARY, 0);
	lv_obj_set_style_text_color(text, COLOR_INTERACTIVE_ACTIVE, LV_PART_MAIN| LV_STATE_CHECKED);

	lv_obj_t *right_panel = lv_obj_create(panel);
	lv_obj_remove_style_all(right_panel);
	lv_obj_set_height(right_panel, 100);
	lv_obj_set_width(right_panel, LV_SIZE_CONTENT);
	lv_obj_set_align(right_panel, LV_ALIGN_RIGHT_MID);
	lv_obj_set_flex_flow(right_panel,LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(right_panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_remove_flag(right_panel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE); 
	lv_obj_set_style_pad_left(right_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(right_panel, 30, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(right_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(right_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_column(right_panel, 30, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(right_panel, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t *status_label = lv_label_create(right_panel);
	lv_obj_set_width(status_label, LV_SIZE_CONTENT);
	lv_obj_set_height(status_label, LV_SIZE_CONTENT);
	lv_obj_set_align(status_label, LV_ALIGN_CENTER);
	lv_label_set_text(status_label,"Connected");
	lv_obj_set_style_text_font(status_label, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(status_label, COLOR_TEXT_SECONDARY, 0);
	lv_obj_add_flag(status_label, LV_OBJ_FLAG_HIDDEN);

	lv_obj_t *status_spinner = lv_spinner_create(right_panel);
	lv_obj_set_width(status_spinner, 32);
	lv_obj_set_height(status_spinner, 32);
	lv_obj_set_align(status_spinner, LV_ALIGN_CENTER);
	lv_obj_remove_flag(status_spinner, LV_OBJ_FLAG_CLICKABLE); 
	lv_obj_set_style_arc_color(status_spinner, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(status_spinner, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_arc_width(status_spinner, 5, LV_PART_MAIN| LV_STATE_DEFAULT);

	lv_obj_set_style_arc_color(status_spinner, lv_color_hex(0x1976D2), LV_PART_INDICATOR | LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(status_spinner, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
	lv_obj_set_style_arc_width(status_spinner, 5, LV_PART_INDICATOR| LV_STATE_DEFAULT);
	lv_obj_add_flag(status_spinner, LV_OBJ_FLAG_HIDDEN);

	LV_IMAGE_DECLARE(icon_wifi_unlocked);
	LV_IMAGE_DECLARE(icon_wifi_locked);
	lv_obj_t *type_icon = lv_image_create(right_panel);
	if (wifi->secure){
		lv_image_set_src(type_icon, &icon_wifi_locked);
	} else {
		lv_image_set_src(type_icon, &icon_wifi_unlocked);
	}
	lv_obj_set_width(type_icon, 40);
	lv_obj_set_height(type_icon, LV_SIZE_CONTENT);
	lv_obj_set_align(type_icon, LV_ALIGN_CENTER);
	lv_obj_add_flag(type_icon, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_remove_flag(type_icon, LV_OBJ_FLAG_SCROLLABLE); 

	LV_IMAGE_DECLARE(icon_chevron_right);
	lv_obj_t *more_icon = lv_image_create(right_panel);
	lv_image_set_src(more_icon, &icon_chevron_right);
	lv_obj_set_width(more_icon, 40);
	lv_obj_set_height(more_icon, LV_SIZE_CONTENT);
	lv_obj_set_align(more_icon, LV_ALIGN_RIGHT_MID);
	lv_obj_add_flag(more_icon, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_remove_flag(more_icon, LV_OBJ_FLAG_SCROLLABLE); 
	lv_obj_set_style_image_recolor(more_icon, lv_color_hex(0xFFFFFF), LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_image_recolor_opa(more_icon, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_add_flag(more_icon, LV_OBJ_FLAG_HIDDEN);

	if (wifi->connected){
		lv_obj_add_state(signal_icon, LV_STATE_CHECKED);
		lv_obj_add_state(text, LV_STATE_CHECKED);

		lv_obj_remove_flag(status_label, LV_OBJ_FLAG_HIDDEN);
		lv_obj_remove_flag(more_icon, LV_OBJ_FLAG_HIDDEN);
	}

	lv_obj_add_event_cb(panel, ui_open_wifi_details, LV_EVENT_CLICKED, wifi);

	return panel;
}

static void ui_textarea_event_cb(lv_event_t *e)
{
	lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);

	lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
	if(label_draw_dsc) {
		/* Offset of the placeholder text */
		label_draw_dsc->ofs_y = 5;
	}
}


static void wifi_anim_exec_cb(lv_anim_t* a, int32_t v)
{

}

static void wifi_anim_complete_cb(lv_anim_t *a)
{
	/* Simulate wifi connection request result after 3 secs */	

	int32_t state = lv_obj_get_index(selected_wifi_item) % 5;
	if (state == 1){
		selected_wifi_detail->connected = true;
		selected_wifi_detail->ip = "192.168.0.1";
	}

	/* Update the wifi status. Should typically happen from the hardware */
	wifi_update_status(selected_wifi_detail, state);

}

static void wifi_anim_start()
{
    lv_anim_init(&wifi_anim);
    lv_anim_set_values(&wifi_anim, 0, 10);
    lv_anim_set_duration(&wifi_anim, 3000);
	lv_anim_set_custom_exec_cb(&wifi_anim, wifi_anim_exec_cb);
	lv_anim_set_completed_cb(&wifi_anim, wifi_anim_complete_cb);
    lv_anim_start(&wifi_anim);
}

#if LV_USE_SDL
void wifi_selected_cb(wifi_detail_t *wifi)
{
	LV_LOG_WARN("Wi-Fi selected %s -> password %s", wifi->ssid, wifi->secure ? wifi->password : "");

	wifi_anim_start();

}
#endif