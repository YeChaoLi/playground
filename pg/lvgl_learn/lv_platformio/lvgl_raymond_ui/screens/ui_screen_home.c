/**
 * @file ui_screen_home.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../ui.h"
#include "ui_screens.h"
#include "../components/ui_button.h"
#include "../components/ui_spinbox.h"
#include "../components/ui_roller.h"
#include "../components/ui_switch.h"
#include "../components/ui_label.h"
#include "../components/ui_footer.h"
#include "../components/ui_list_element.h"
#include "../components/ui_title_bar.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

#define MAX_PRESETS 13
#define MAX_CUSTOM_PRESETS 9
#define MAX_CUSTOM_PRESET_NAME_LENGTH 20

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/

static void ui_screen_home_loaded(lv_event_t * e);
static void ui_screen_home_unloaded(lv_event_t * e);
static void ui_footer_events(lv_event_t *e);
static void ui_pre_heat_info(lv_event_t *e);

static void ui_title_bar_event(lv_event_t * e);

static void ui_preset_click(lv_event_t *e);
static void ui_preset_selected(lv_event_t *e);

static void heat_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void cure_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void ui_switch_clicked_event(lv_event_t *e);

static void add_cure_preset(lv_obj_t * parent, cure_preset_t preset);
static void set_cure_preset(cure_preset_t preset);
static void update_switch(lv_obj_t * switch_obj, bool state, bool send_event);

static void list_remove_info(lv_obj_t * parent);
static void list_add_info(lv_obj_t * parent);
static void list_populate();
static void list_populate_subtype(cure_preset_t *preset);
static void check_show_pill();

static void update_selected_item(uint32_t image_index, const char * text, bool icon);

static void cure_time_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void cure_icon_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * home_screen;

static lv_obj_t *title_bar;
static lv_obj_t *title_bar_text;
static lv_obj_t *list_cont;
static lv_obj_t *list_bg;
static lv_obj_t *selected;
static lv_obj_t *spinbox_cont;
static lv_obj_t *main_cont;

static lv_obj_t *preset_pill;
static lv_obj_t *preset_pill_text;

static lv_obj_t *pre_info_icon;
static lv_obj_t *pre_cont;
static lv_obj_t *pre_label;
// static lv_obj_t * pre_switch;
static lv_obj_t *pre_time_spinbox;
static lv_obj_t *pre_time_label;
static lv_obj_t *pre_temp_spinbox;
static lv_obj_t *pre_temp_label;

static lv_obj_t *cure_cont;
// static lv_obj_t * cure_switch;
static lv_obj_t *cure_time_spinbox;
static lv_obj_t *cure_temp_spinbox;

static uint32_t selected_position;
static bool edit_mode;
static bool create_mode;

static ui_footer_t * footer;

static cure_preset_t types[] = {
	{.name = "Normal", .icon = NULL, .pre_time = 6, .cure_time = 16, .cure_temp = 47},
	{.name = "Fast", .icon = NULL, .pre_time = 2, .cure_time = 6, .cure_temp = 72},
	{.name = "Max Elongation", .icon = NULL, .pre_time = 3, .cure_time = 22, .cure_temp = 33},
};

static cure_preset_t version[] = {
	{.name = "V3", .subtype = types, .subtype_size = 3},
	{.name = "V4", .subtype = types, .subtype_size = 3},
	{.name = "V4.1", .subtype = types, .subtype_size = 0, .pre_time = 5, .cure_time = 4, .cure_temp = 50},
	{.name = "V5", .subtype = types, .subtype_size = 2},
};

static cure_preset_t cure_presets[] = {
	{.name = "Black", .icon = &image_droplet_black_icon, .subtype = version, .subtype_size = 4},
	{.name = "Clear", .icon = &image_droplet_clear_icon, .subtype = version, .subtype_size = 0, .pre_time = 8, .cure_time = 14, .cure_temp = 56},
	{.name = "Grey", .icon = &image_droplet_grey_icon, .subtype = version, .subtype_size = 2},
	{.name = "White", .icon = &image_droplet_white_icon, .subtype = version, .subtype_size = 4},
};

static cure_preset_t custom_presets[MAX_PRESETS];

static current_preset_t current_selected_preset;

static char custom_names[MAX_CUSTOM_PRESETS][MAX_CUSTOM_PRESET_NAME_LENGTH];
static char type_output[20];
static char last_preset_subtype[20];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_screen_home_init(void)
{

	home_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(home_screen, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(home_screen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(home_screen, LV_OBJ_FLAG_SCROLLABLE);

	LV_FONT_DECLARE(font_proxima_bold_35);
	
	main_cont = lv_obj_create(home_screen);
	lv_obj_remove_style_all(main_cont);
	lv_obj_set_width(main_cont, 662);
	lv_obj_set_height(main_cont, 256);
	lv_obj_set_x(main_cont, 598);
	lv_obj_set_y(main_cont, 20);
	lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(main_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

	pre_cont = lv_obj_create(main_cont);
	lv_obj_set_width(pre_cont, 231);
	lv_obj_set_height(pre_cont, 254);
	lv_obj_set_align(pre_cont, LV_ALIGN_CENTER);
	lv_obj_remove_flag(pre_cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_radius(pre_cont, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(pre_cont, COLOR_SURFACE, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(pre_cont, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(pre_cont, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(pre_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(pre_cont, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(pre_cont, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(pre_cont, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(pre_cont, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(pre_cont, lv_color_hex(0x0E0E0E), LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_set_style_bg_opa(pre_cont, 255, LV_PART_MAIN | LV_STATE_DISABLED);


	// pre_switch = ui_switch_create(pre_cont);
	// lv_obj_set_align(pre_switch, LV_ALIGN_TOP_RIGHT);
	// lv_obj_set_x(pre_switch, -4);
	// lv_obj_set_y(pre_switch, 12);
	// lv_obj_add_state(pre_switch, LV_STATE_CHECKED);
	// lv_button_bind_checked(pre_switch, &subject_switch_heat);
	// lv_obj_add_event_cb(pre_switch, ui_switch_clicked_event, LV_EVENT_CLICKED, NULL);
	// lv_obj_set_state(pre_switch, LV_STATE_DISABLED, true); // DISABLE FOR TESTING


	spinbox_cont = lv_obj_create(pre_cont);
	lv_obj_remove_style_all(spinbox_cont);
	lv_obj_set_width(spinbox_cont, 167);
	lv_obj_set_height(spinbox_cont, 180);
	lv_obj_set_align(spinbox_cont, LV_ALIGN_BOTTOM_MID);
	lv_obj_remove_flag(spinbox_cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_text_color(spinbox_cont, lv_color_hex(0xffffff), 0);
	lv_obj_set_flex_flow(spinbox_cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(spinbox_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_column(spinbox_cont, 4, 0);

	lv_obj_t *spinbox;
	pre_time_spinbox = ui_roller_create(spinbox_cont, false, &subject_time_heat);
	lv_obj_add_event_cb(lv_obj_get_child(pre_time_spinbox, 1), ui_switch_clicked_event, LV_EVENT_VALUE_CHANGED, NULL);

	LV_FONT_DECLARE(font_rubik_28);
	LV_FONT_DECLARE(font_rubik_90);
	LV_FONT_DECLARE(font_proxima_24);
	LV_FONT_DECLARE(font_proxima_28);
	lv_obj_t *label;
	pre_time_label = ui_label_create(spinbox_cont, "min", LV_SIZE_CONTENT, 32);
	lv_obj_set_style_text_font(pre_time_label, &font_proxima_24, 0);

	// pre_temp_spinbox = ui_roller_create(spinbox_cont, true, &subject_temperature_heat);
	// lv_obj_add_event_cb(lv_obj_get_child(pre_temp_spinbox, 1), ui_switch_clicked_event, LV_EVENT_VALUE_CHANGED, NULL);

	// pre_temp_label = ui_label_create(spinbox_cont, "°C", LV_SIZE_CONTENT, 32);
	// lv_obj_set_style_text_font(pre_temp_label, &font_proxima_24, 0);


	lv_obj_t *pre_title = lv_obj_create(pre_cont);
	lv_obj_remove_style_all(pre_title);
	lv_obj_set_width(pre_title, LV_SIZE_CONTENT);
	lv_obj_set_height(pre_title, 40);
	lv_obj_set_align(pre_title, LV_ALIGN_TOP_MID);
	lv_obj_set_flex_flow(pre_title, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(pre_title, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_column(pre_title, 4, 0);

	pre_label = ui_label_create(pre_title, "Preheat", LV_SIZE_CONTENT, 40);
	lv_obj_set_style_text_font(pre_label, &font_proxima_28, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(pre_label, COLOR_TEXT_SECONDARY, 0);

	LV_IMAGE_DECLARE(image_info_icon);
	pre_info_icon = lv_image_create(pre_title);
	lv_image_set_src(pre_info_icon, &image_info_icon);
	lv_obj_set_width(pre_info_icon, LV_SIZE_CONTENT);
	lv_obj_set_height(pre_info_icon, LV_SIZE_CONTENT);
	lv_obj_set_align(pre_info_icon, LV_ALIGN_CENTER);
	lv_obj_add_flag(pre_info_icon, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_remove_flag(pre_info_icon, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_add_event_cb(pre_info_icon, ui_pre_heat_info, LV_EVENT_CLICKED, NULL);
	
	cure_cont = lv_obj_create(main_cont);
	lv_obj_set_width(cure_cont, 411);
	lv_obj_set_height(cure_cont, 254);
	lv_obj_set_align(cure_cont, LV_ALIGN_CENTER);
	lv_obj_remove_flag(cure_cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_color(cure_cont, COLOR_SURFACE, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(cure_cont, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(cure_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(cure_cont, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(cure_cont, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(cure_cont, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(cure_cont, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(cure_cont, lv_color_hex(0x0E0E0E), LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_set_style_bg_opa(cure_cont, 255, LV_PART_MAIN | LV_STATE_DISABLED);

	// cure_switch = ui_switch_create(cure_cont);
	// lv_obj_set_align(cure_switch, LV_ALIGN_TOP_RIGHT);
	// lv_obj_set_x(cure_switch, -4);
	// lv_obj_set_y(cure_switch, 12);
	// lv_obj_add_state(cure_switch, LV_STATE_CHECKED);
	// lv_button_bind_checked(cure_switch, &subject_switch_cure);
	// lv_obj_add_event_cb(cure_switch, ui_switch_clicked_event, LV_EVENT_CLICKED, NULL);

	spinbox_cont = lv_obj_create(cure_cont);
	lv_obj_remove_style_all(spinbox_cont);
	lv_obj_set_width(spinbox_cont, 347);
	lv_obj_set_height(spinbox_cont, 180);
	lv_obj_remove_flag(spinbox_cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_align(spinbox_cont, LV_ALIGN_BOTTOM_MID);
	lv_obj_set_style_text_color(spinbox_cont, lv_color_hex(0xffffff), 0);
	lv_obj_set_flex_flow(spinbox_cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(spinbox_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_column(spinbox_cont, 4, 0);

	lv_obj_t *cure_label = ui_label_create(cure_cont, "Cure", LV_SIZE_CONTENT, 40);
	lv_obj_set_align(lv_obj_get_parent(cure_label), LV_ALIGN_TOP_MID);
	lv_obj_set_style_text_font(cure_label, &font_proxima_28, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(cure_label, COLOR_TEXT_SECONDARY, 0);

	cure_time_spinbox = ui_roller_create(spinbox_cont, false, &subject_time_cure);
	lv_obj_add_event_cb(lv_obj_get_child(cure_time_spinbox, 1), ui_switch_clicked_event, LV_EVENT_VALUE_CHANGED, NULL);

	label = ui_label_create(spinbox_cont, "min", LV_SIZE_CONTENT, 32);
	lv_label_set_text(label, "min");
	lv_obj_set_style_text_font(label, &font_proxima_24, 0);

	cure_temp_spinbox = ui_roller_create(spinbox_cont, true, &subject_temperature_cure);
	lv_obj_add_event_cb(lv_obj_get_child(cure_temp_spinbox, 1), ui_switch_clicked_event, LV_EVENT_VALUE_CHANGED, NULL);

	label = ui_label_create(spinbox_cont, "°C", LV_SIZE_CONTENT, 32);
	lv_label_set_text(label, "°C");
	lv_obj_set_style_text_font(label, &font_proxima_24, 0);

	/* Footer object */
	footer = ui_footer_create(home_screen, FT_SETTINGS | FT_CURE | FT_START, ui_footer_events);
	
	LV_IMAGE_DECLARE(image_droplet);
	LV_IMAGE_DECLARE(image_custom_preset_icon);
	LV_IMAGE_DECLARE(image_plus_icon);

	list_bg = lv_obj_create(home_screen);
	lv_obj_remove_style_all(list_bg);
	lv_obj_set_size(list_bg, 558, lv_pct(100));
	lv_obj_align(list_bg, LV_ALIGN_TOP_LEFT, 20, 0);
	lv_obj_add_flag(list_bg, LV_OBJ_FLAG_HIDDEN);
	lv_obj_set_style_bg_color(list_bg, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(list_bg, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

	selected = ui_list_element_create(home_screen, NULL, "No preset selected");
	lv_label_bind_text(lv_obj_get_child(selected, 1), &subject_preset_type, NULL);
	lv_obj_set_size(selected, 558, 84);
	lv_obj_set_pos(selected, 20, 132);
	lv_obj_add_state(selected, LV_STATE_CHECKED);
	lv_obj_add_event_cb(selected, ui_preset_click, LV_EVENT_CLICKED, NULL);

	list_cont = lv_obj_create(home_screen);
	lv_obj_remove_style_all(list_cont);
	lv_obj_set_size(list_cont, 558, 400 + 48);
	lv_obj_set_flex_flow(list_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_scroll_snap_y(list_cont, LV_SCROLL_SNAP_CENTER);
	lv_obj_align(list_cont, LV_ALIGN_TOP_LEFT, 20, -48);
	lv_obj_add_flag(list_cont, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(list_cont, LV_OBJ_FLAG_CLICK_FOCUSABLE);
	lv_obj_add_flag(list_cont, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_set_style_pad_left(list_cont, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(list_cont, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(list_cont, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(list_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

	title_bar = ui_title_bar_create(home_screen, "Create preset");
	lv_obj_set_width(title_bar, 598);
	title_bar_text = lv_obj_get_child(title_bar, 1);
	lv_obj_set_style_text_font(title_bar_text, &font_proxima_bold_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_add_event_cb(title_bar, ui_title_bar_event, LV_EVENT_CLICKED, NULL);
	lv_obj_add_flag(title_bar, LV_OBJ_FLAG_HIDDEN);
	lv_obj_set_style_bg_color(title_bar, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(title_bar, 255, LV_PART_MAIN | LV_STATE_DEFAULT);


	preset_pill = lv_obj_create(home_screen);
	lv_obj_set_width(preset_pill, LV_SIZE_CONTENT);
	lv_obj_set_height(preset_pill, 50);
	lv_obj_align_to(preset_pill, selected, LV_ALIGN_RIGHT_MID, -40, 0);
	lv_obj_remove_flag(preset_pill, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_radius(preset_pill, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(preset_pill, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(preset_pill, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(preset_pill, lv_color_hex(0x1976D2), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(preset_pill, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(preset_pill, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(preset_pill, LV_OBJ_FLAG_CLICKABLE);

	preset_pill_text = lv_label_create(preset_pill);
	lv_obj_set_width(preset_pill_text, LV_SIZE_CONTENT);
	lv_obj_set_height(preset_pill_text, LV_SIZE_CONTENT); /// 40
	lv_obj_set_align(preset_pill_text, LV_ALIGN_CENTER);
	lv_label_set_long_mode(preset_pill_text, LV_LABEL_LONG_CLIP);
	lv_label_set_text(preset_pill_text, "V4 • Norm");
	lv_obj_set_style_text_font(preset_pill_text, &font_proxima_28, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(preset_pill_text, COLOR_TEXT_PRIMARY, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_remove_flag(preset_pill_text, LV_OBJ_FLAG_CLICKABLE);

	lv_obj_add_event_cb(list_cont, ui_preset_selected, LV_EVENT_ALL, NULL);

	list_populate();

	current_selected_preset.level = 0;

	lv_subject_add_observer(&subject_time_cure, cure_time_observer_cb, NULL);

	lv_subject_add_observer(&subject_switch_heat, heat_switch_observer_cb, NULL);
	lv_subject_add_observer(&subject_switch_cure, cure_switch_observer_cb, NULL);
	lv_subject_add_observer(&subject_preset_icon, cure_icon_observer_cb, NULL);

	lv_obj_add_event_cb(home_screen, ui_screen_home_loaded, LV_EVENT_SCREEN_LOADED, NULL);
	lv_obj_add_event_cb(home_screen, ui_screen_home_unloaded, LV_EVENT_SCREEN_UNLOADED, NULL);

	// lv_subject_set_int(&subject_switch_heat, 0); // off by default
}

lv_obj_t * ui_screen_home(void)
{
	if (!home_screen){
		ui_screen_home_init();
	}
	return home_screen;
}

ui_footer_t *ui_footer_home(void)
{
	if (!home_screen){
		ui_screen_home_init();
	}
	return footer;
}

void add_custom_preset(cure_preset_t preset, uint32_t index)
{
	if (index && index < 9){
		custom_presets[index] = preset;
	}

	list_populate();
	
}

void load_preset(uint32_t index)
{
	if (index && index < 4 + MAX_CUSTOM_PRESETS) {
		if (index < 3 ){
			set_cure_preset(cure_presets[index]);
		} else if (custom_presets[index - 4].enabled){
			set_cure_preset(custom_presets[index - 4]);
		}
	}
}

void show_home_screen(void)
{
	lv_screen_load(ui_screen_home());
}

#if LV_USE_SDL
bool get_cure_profile_info_from_index_cb(uint32_t index, cure_preset_t *cure_profile)
{
	// cure_profile = &cure_presets[index];
	cure_profile->name = cure_presets[index].name;
	cure_profile->icon = cure_presets[index].icon;
	cure_profile->pre_time = cure_presets[index].pre_time;
	cure_profile->cure_time = cure_presets[index].cure_time;
	cure_profile->cure_temp = cure_presets[index].cure_temp;
	cure_profile->subtype = cure_presets[index].subtype;
	cure_profile->subtype_size = cure_presets[index].subtype_size;
	cure_profile->enabled = cure_presets[index].enabled;
	
	return false;
}

size_t get_number_of_preset_profiles_cb()
{
	return 4;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ui_screen_home_loaded(lv_event_t * e) {
    on_screen_loaded_cb(SCREEN_HOME);
}

static void ui_screen_home_unloaded(lv_event_t * e) {

	for (uint32_t i = 0; i < lv_obj_get_child_count(selected); i++) {
		lv_obj_remove_flag(lv_obj_get_child(selected, i), LV_OBJ_FLAG_HIDDEN);
	}

	lv_obj_add_flag(list_bg, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(list_cont, LV_OBJ_FLAG_HIDDEN);

	// list_populate();

	lv_label_set_text(preset_pill_text, last_preset_subtype);

	if (lv_obj_has_flag(preset_pill, LV_OBJ_FLAG_USER_1)){
		lv_obj_align_to(preset_pill, selected, LV_ALIGN_RIGHT_MID, -40, 0);
		lv_obj_remove_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);
	} else {
		lv_obj_add_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);
	}

	lv_obj_add_flag(title_bar, LV_OBJ_FLAG_HIDDEN);

	// current_selected_preset.level = 0;
	// current_selected_preset.current = NULL;

}

static void list_populate()
{
	lv_obj_clean(list_cont);

	// add preset profiles
	size_t num_of_presets = get_number_of_preset_profiles_cb();
	for (int i = 0; i < num_of_presets; i++){
		cure_preset_t preset = {0};
		get_cure_profile_info_from_index_cb(i, &preset);  // get from curecycle manager
		add_cure_preset(list_cont, preset);
	}

	// add custom profiles
	for (int i = 0; i < MAX_CUSTOM_PRESETS; i++){
		if (custom_presets[i].enabled){
			snprintf(custom_names[i], sizeof(custom_names[i]), "Custom %d", i + 1);
			custom_presets[i].name = custom_names[i];
			add_cure_preset(list_cont, custom_presets[i]);
		}
	}
	list_add_info(list_cont);

	lv_obj_update_snap(list_cont, LV_ANIM_OFF);

	lv_obj_scroll_to_view(lv_obj_get_child(list_cont, 0), LV_ANIM_OFF);
}

static void list_populate_subtype(cure_preset_t *preset)
{
	lv_obj_clean(list_cont);

	for (int i = 0; i < preset->subtype_size; i++)
	{
		preset->subtype[i].icon = current_selected_preset.main->icon;
		add_cure_preset(list_cont, preset->subtype[i]);
	}
	lv_obj_update_snap(list_cont, LV_ANIM_OFF);

	lv_obj_scroll_to_view(lv_obj_get_child(list_cont, 0), LV_ANIM_OFF);
}

static void list_remove_info(lv_obj_t * parent)
{
	/* Remove last info item */
	lv_obj_t * info = lv_obj_get_child(parent, lv_obj_get_child_count(parent) - 1);
	if (info != NULL && lv_obj_check_type(info, &lv_label_class))
	{
		lv_obj_delete(info);
	}
	
	lv_obj_t * custom = lv_obj_get_child(parent, lv_obj_get_child_count(parent) - 1);
	if (custom != NULL){
		if (lv_obj_get_child_count(custom) >= 2 && lv_obj_has_flag(lv_obj_get_child(custom, 2), LV_OBJ_FLAG_HIDDEN)){
			lv_obj_delete(custom);
		}
	}
	
}

static void update_selected_item(uint32_t image_index, const char * text, bool icon)
{
	lv_subject_set_int(&subject_preset_icon, image_index);
	lv_subject_copy_string(&subject_preset_type, text);
	lv_subject_copy_string(&subject_preset_type_text, text);

	if (icon){
		lv_obj_remove_flag(lv_obj_get_child(selected, 2), LV_OBJ_FLAG_HIDDEN);
	} else {
		lv_obj_add_flag(lv_obj_get_child(selected, 2), LV_OBJ_FLAG_HIDDEN);
	}

	lv_obj_add_flag(title_bar, LV_OBJ_FLAG_HIDDEN);
}

static void list_add_info(lv_obj_t * parent)
{
	LV_IMAGE_DECLARE(image_plus_icon);
	if (lv_obj_get_child_count(parent) < get_number_of_preset_profiles_cb() + MAX_CUSTOM_PRESETS){
		printf("Added create preset button\n");
		lv_obj_t * custom = ui_list_element_create(parent, &image_plus_icon, "Create Preset");
		lv_obj_add_flag(lv_obj_get_child(custom, 2), LV_OBJ_FLAG_HIDDEN);
	}
	
	LV_FONT_DECLARE(font_proxima_24);
	lv_obj_t *info_label = lv_label_create(parent);
	lv_obj_set_width(info_label, lv_pct(100));
	lv_label_set_long_mode(info_label, LV_LABEL_LONG_WRAP);
	lv_label_set_text(info_label, "For most recent material presets visit support.formlabs.com");
	lv_obj_set_style_text_font(info_label, &font_proxima_24, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(info_label, COLOR_TEXT_SECONDARY, 0);
	lv_obj_set_style_pad_left(info_label, 40, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(info_label, 20, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(info_label, 20, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(info_label, LV_OBJ_FLAG_SNAPPABLE);
	lv_obj_remove_flag(info_label, LV_OBJ_FLAG_EVENT_BUBBLE);
	lv_obj_add_flag(info_label, LV_OBJ_FLAG_CLICKABLE);
}

static void ui_title_bar_event(lv_event_t * e)
{
	if (edit_mode){
		edit_mode = false;
		ui_footer_toggle_buttons(footer, FT_SETTINGS | FT_EDIT | FT_CURE | FT_START);
		lv_obj_add_flag(title_bar, LV_OBJ_FLAG_HIDDEN);
	}
	if (create_mode){
		create_mode = false;
		ui_footer_toggle_buttons(footer, FT_SETTINGS | FT_CURE | FT_START);
		update_selected_item(0, "No preset selected", true);

	}

	if (current_selected_preset.level > 0){
		if (current_selected_preset.level == 1)
		{
			current_selected_preset.current = NULL;
			list_populate();
			lv_obj_add_flag(title_bar, LV_OBJ_FLAG_HIDDEN);
			lv_obj_scroll_to_view(lv_obj_get_child(list_cont, current_selected_preset.initial), LV_ANIM_OFF);
		} else if (current_selected_preset.level == 2)
		{
			current_selected_preset.current = current_selected_preset.main;
			list_populate_subtype(current_selected_preset.current);
			lv_obj_scroll_to_view(lv_obj_get_child(list_cont, current_selected_preset.version_pos), LV_ANIM_OFF);
			
		}
		current_selected_preset.level--;
		check_show_pill();
	}
}

static void ui_footer_events(lv_event_t *e)
{
	footer_button_t button = (footer_button_t)(intptr_t)lv_event_get_user_data(e);

	/* Handle button pressed actions from the callback */
	on_footer_button_pressed_cb(SCREEN_HOME, button);
	
	/* Handle button pressed actions in this file */
	switch (button){
		case FT_SETTINGS:
			lv_screen_load(ui_screen_settings());
		break;
		case FT_CURE:
			update_selected_item(0, "No preset selected", true);
			lv_obj_add_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);
			lv_obj_remove_flag(preset_pill, LV_OBJ_FLAG_USER_1);
			// lv_obj_scroll_to_view(lv_obj_get_child(list_cont, 0), LV_ANIM_OFF);
			

			// update_switch(cure_switch, true, true);
			// update_switch(pre_switch, false, true);
			lv_subject_set_int(&subject_time_heat, 0);
			lv_subject_set_int(&subject_time_cure, 1);
			lv_subject_set_int(&subject_temperature_cure, 60);
			ui_footer_toggle_buttons(footer, FT_SETTINGS | FT_CURE | FT_START);
#if LV_USE_SDL
			lv_screen_load(ui_screen_curing());
#endif
		break;
		case FT_START:
#if LV_USE_SDL
			if (lv_subject_get_int(&subject_drying_mode_switch))
			{
				lv_screen_load(ui_screen_drying());
			}
			else
			{
				if (lv_subject_get_int(&subject_time_heat) && lv_subject_get_int(&subject_temperature_cure) > 34)
				{
					lv_screen_load(ui_screen_heat_cool());
				}
				else
				{
					lv_screen_load(ui_screen_curing());
				}
			}
#endif
		break;
		case FT_SAVE:

			if (create_mode){
				create_mode = false;
			}

			custom_presets[selected_position].pre_time = lv_subject_get_int(&subject_time_heat);
			// custom_presets[selected_position].pre_temp = lv_subject_get_int(&subject_temperature_heat);
			// custom_presets[selected_position].pre_switch = lv_obj_has_state(pre_switch, LV_STATE_CHECKED);
			custom_presets[selected_position].cure_time = lv_subject_get_int(&subject_time_cure);
			custom_presets[selected_position].cure_temp = lv_subject_get_int(&subject_temperature_cure);
			// custom_presets[selected_position].cure_switch = lv_obj_has_state(cure_switch, LV_STATE_CHECKED);
			ui_footer_toggle_buttons(footer, FT_SETTINGS | FT_EDIT | FT_CURE | FT_START);
			lv_obj_add_flag(title_bar, LV_OBJ_FLAG_HIDDEN);

			if (edit_mode){
				edit_mode = false;
			} else {
				custom_presets[selected_position].enabled = true;
				on_save_custom_preset_cb(custom_presets[selected_position], selected_position);
				list_populate();
				
				lv_obj_t * item = lv_obj_get_child(list_cont, selected_position + get_number_of_preset_profiles_cb());
				if (item != NULL){
					lv_obj_scroll_to_view(item, LV_ANIM_ON);
				}

				lv_image_set_src(lv_obj_get_child(selected, 0), custom_presets[selected_position].icon);
				lv_label_set_text_fmt(lv_obj_get_child(selected, 1), "Custom %d", selected_position + 1);
				lv_obj_remove_flag(lv_obj_get_child(selected, 2), LV_OBJ_FLAG_HIDDEN);
				
			}
		break;
		case FT_EDIT:
			edit_mode = true;
			ui_footer_toggle_buttons(footer, FT_SAVE | FT_DELETE);
			lv_label_set_text(title_bar_text, "Edit preset");
			lv_obj_remove_flag(title_bar, LV_OBJ_FLAG_HIDDEN);
			on_save_custom_preset_cb(custom_presets[selected_position], selected_position);
			break;
		case FT_DELETE:
			edit_mode = false;
			custom_presets[selected_position].enabled = false;
			on_delete_custom_preset_cb(selected_position);
			list_populate();

			ui_footer_toggle_buttons(footer, FT_SETTINGS | FT_CURE | FT_START);
			update_selected_item(0, "No preset selected", true);
			lv_obj_add_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);
			lv_obj_remove_flag(preset_pill, LV_OBJ_FLAG_USER_1);

			break;
		default:
			break;
	}

}

static void ui_pre_heat_info(lv_event_t *e)
{
	lv_screen_load(ui_screen_pre_heat());
}

static void add_cure_preset(lv_obj_t * parent, cure_preset_t preset)
{
	lv_obj_t * item = ui_list_element_create(parent, preset.icon, preset.name);
	if (!preset.enabled){
		lv_obj_add_flag(lv_obj_get_child(item, 2), LV_OBJ_FLAG_HIDDEN);
	}
}

static void set_cure_preset(cure_preset_t preset)
{
	lv_subject_set_int(&subject_time_heat, preset.pre_time);
	lv_subject_set_int(&subject_time_cure, preset.cure_time);
	lv_subject_set_int(&subject_temperature_cure, preset.cure_temp);
}

static void ui_preset_click(lv_event_t *e)
{
	lv_obj_remove_flag(list_bg, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(list_cont, LV_OBJ_FLAG_HIDDEN);
	for (uint32_t i = 0; i < lv_obj_get_child_count(selected); i++) {
		lv_obj_add_flag(lv_obj_get_child(selected, i), LV_OBJ_FLAG_HIDDEN);
	}
	lv_obj_add_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);
	// current_selected_preset.level = 0; /* Reset level on click selector */

	check_show_pill();
}

static void ui_preset_selected(lv_event_t *e)
{
	
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t * list = lv_event_get_current_target(e); /* List that contains the items */
    lv_obj_t * item = lv_event_get_target(e); /* Item that was clicked */
    if(event_code == LV_EVENT_CLICKED) {
		if (list != item){
			/* Make sure an item was clicked and not the list itself */
			uint32_t total_items = lv_obj_get_child_count(list);
			uint32_t item_pos = 0;
			
			for(uint32_t i = 0; i < lv_obj_get_child_count(list); i++) {
				if (item == lv_obj_get_child(list, i)){
					item_pos = i;
				}

			}

			

			lv_obj_scroll_to_view(item, LV_ANIM_OFF);

			LV_LOG_WARN("List item clicked %d", item_pos);
			edit_mode = false;

			if (current_selected_preset.level <= 0)
			{
				uint32_t offset = 2;
				if (!lv_obj_has_flag(lv_obj_get_child(item, 2), LV_OBJ_FLAG_HIDDEN)){
					offset = 1;
					LV_LOG_WARN("Last item not create preset");
				} else {
					LV_LOG_WARN("Last item is create preset");
				}

				create_mode = total_items - offset == item_pos;

				if (!create_mode){
					on_preset_selected_cb(item_pos);
				}

				if (create_mode){

					update_selected_item(
						item_pos + 1, 
						lv_label_get_text(lv_obj_get_child(item, 1)), 
						!lv_obj_has_flag(lv_obj_get_child(item, 2), LV_OBJ_FLAG_HIDDEN)
					);

					ui_footer_toggle_buttons(footer, FT_SAVE);
					LV_LOG_WARN("Create a new preset");
					for (int i = 0; i < MAX_CUSTOM_PRESETS; i++){
						if (!custom_presets[i].enabled){

							snprintf(custom_names[i], sizeof(custom_names[i]), "Custom %d", i + 1);
							custom_presets[i].name = custom_names[i];
							custom_presets[i].icon = &image_custom_preset_icon;

							update_selected_item(5, custom_presets[i].name, true);

							selected_position = i;
							break;
						}
					}
					lv_label_set_text(title_bar_text, "Create preset");
					lv_obj_remove_flag(title_bar, LV_OBJ_FLAG_HIDDEN);

					/* Update the selected item text in the main screen */
					for (uint32_t i = 0; i < lv_obj_get_child_count(selected) - 1; i++)
					{
						lv_obj_remove_flag(lv_obj_get_child(selected, i), LV_OBJ_FLAG_HIDDEN);
					}
				} else if (item_pos < get_number_of_preset_profiles_cb()){
					if (current_selected_preset.current == NULL)
					{
						LV_LOG_WARN("Current preset is NULL");
						current_selected_preset.initial = item_pos;
						static cure_preset_t preset = {0};
						get_cure_profile_info_from_index_cb(item_pos, &preset);

						current_selected_preset.current = &preset;
						current_selected_preset.main = &preset;
					}

					LV_LOG_WARN("Inbuilt preset selected %d of %s, subtypes %zu", item_pos, current_selected_preset.current->name, current_selected_preset.current->subtype_size);
					
					if (current_selected_preset.current->subtype_size > 0)
					{
						/* subtypes are present */
						/* Regenerate list, enable header for back navigation & set current preset */
						lv_obj_add_flag(lv_obj_get_child(selected, 2), LV_OBJ_FLAG_HIDDEN);

						list_populate_subtype(current_selected_preset.current);
						
						/* Increment level, (subtype present) */
						current_selected_preset.level++;

						check_show_pill();

						return;
					}
					else
					{
						update_selected_item(
							item_pos + 1, 
							lv_label_get_text(lv_obj_get_child(item, 1)), 
							!lv_obj_has_flag(lv_obj_get_child(item, 2), LV_OBJ_FLAG_HIDDEN)
						);

						ui_footer_toggle_buttons(footer, FT_SETTINGS | FT_CURE | FT_START);
						cure_preset_t preset = {0};
						get_cure_profile_info_from_index_cb(item_pos, &preset);
						set_cure_preset(preset);
						LV_LOG_WARN("Inbuilt preset selected %d", item_pos);

						current_selected_preset.current = NULL;
						/* Repopulate with initial list pos */
						// list_populate();
						// lv_obj_scroll_to_view(lv_obj_get_child(list_cont, current_selected_preset.initial), LV_ANIM_ON);

						/* Update the selected item text in the main screen */
						for (uint32_t i = 0; i < lv_obj_get_child_count(selected) - 1; i++)
						{
							lv_obj_remove_flag(lv_obj_get_child(selected, i), LV_OBJ_FLAG_HIDDEN);
						}
						lv_obj_remove_flag(preset_pill, LV_OBJ_FLAG_USER_1);

					}
				} else {
					// custom presets
					update_selected_item(
						item_pos + 1, 
						lv_label_get_text(lv_obj_get_child(item, 1)), 
						!lv_obj_has_flag(lv_obj_get_child(item, 2), LV_OBJ_FLAG_HIDDEN)
					);

					ui_footer_toggle_buttons(footer, FT_SETTINGS | FT_EDIT | FT_CURE | FT_START);
					selected_position = item_pos - get_number_of_preset_profiles_cb();
					uint32_t count = 0;
					for (uint32_t i = 0; i < MAX_CUSTOM_PRESETS; i++){
						if (custom_presets[i].enabled){
							if (count == selected_position)
							{
								selected_position = i;
								LV_LOG_WARN("Enabled item pos %d", i);
								break;
							}
							count++;
						}
					}
					set_cure_preset(custom_presets[selected_position]);
					LV_LOG_WARN("Custom preset selected %d, count %d", selected_position, count);

					/* Update the selected item text in the main screen */
					for (uint32_t i = 0; i < lv_obj_get_child_count(selected) - 1; i++)
					{
						lv_obj_remove_flag(lv_obj_get_child(selected, i), LV_OBJ_FLAG_HIDDEN);
					}
					lv_obj_remove_flag(preset_pill, LV_OBJ_FLAG_USER_1);
				}

				lv_obj_add_flag(list_bg, LV_OBJ_FLAG_HIDDEN);
				lv_obj_add_flag(list_cont, LV_OBJ_FLAG_HIDDEN);
			} else {
				/* handle subtypes */

				switch (current_selected_preset.level)
				{
				case 1:
					current_selected_preset.version_pos = item_pos;
					current_selected_preset.version = &current_selected_preset.main->subtype[item_pos]; /* version */
					current_selected_preset.current = &current_selected_preset.main->subtype[item_pos];
					break;
				case 2:
					current_selected_preset.type = &current_selected_preset.version->subtype[item_pos]; /* types */
					current_selected_preset.current = &current_selected_preset.version->subtype[item_pos];
					break;
				}

				LV_LOG_WARN("Subtype preset selected %d of %s, subtypes %zu", item_pos, current_selected_preset.current->name, current_selected_preset.current->subtype_size);
				

				if (current_selected_preset.current->subtype_size > 0)
				{
					/* subtypes are present */
					/* Regenerate list, enable header for back navigation & set current preset */

					lv_obj_add_flag(lv_obj_get_child(selected, 2), LV_OBJ_FLAG_HIDDEN);

					list_populate_subtype(current_selected_preset.current);

					/* Increment level, (subtype present) */
					current_selected_preset.level++;

					LV_LOG_WARN("Current preset level -> %d clicked item", current_selected_preset.level);
					check_show_pill();

				}
				else
				{
					/* no more sub type */
					ui_footer_toggle_buttons(footer, FT_SETTINGS | FT_CURE | FT_START);
					set_cure_preset(*current_selected_preset.current);

					current_selected_preset.current = NULL;

					/* Repopulate with initial list pos */
					// list_populate();
					// lv_obj_scroll_to_view(lv_obj_get_child(list_cont, current_selected_preset.initial), LV_ANIM_ON);

					/* Update the selected item text in the main screen */
					for (uint32_t i = 0; i < lv_obj_get_child_count(selected) - 1; i++)
					{
						lv_obj_remove_flag(lv_obj_get_child(selected, i), LV_OBJ_FLAG_HIDDEN);
					}


					update_selected_item(
						current_selected_preset.initial + 1,
						current_selected_preset.main->name,
						true);


					if (current_selected_preset.level == 2)
					{
						ui_shorten_string(current_selected_preset.type->name, type_output, sizeof(type_output));
						lv_label_set_text_fmt(preset_pill_text, "%s • %s", current_selected_preset.version->name, type_output);

						char text[64];
						snprintf(text, sizeof(text), "%s • %s • %s",
								current_selected_preset.main->name,
								current_selected_preset.version->name,
								current_selected_preset.type->name);

						lv_subject_copy_string(&subject_preset_type_text, text);

					} else if (current_selected_preset.level == 1)
					{
						lv_label_set_text_fmt(preset_pill_text, "%s", current_selected_preset.version->name);

						char text[64];
						snprintf(text, sizeof(text), "%s • %s",
								current_selected_preset.main->name,
								current_selected_preset.version->name);

						lv_subject_copy_string(&subject_preset_type_text, text);
					}

					const char * txt = lv_label_get_text(preset_pill_text);
					lv_strncpy(last_preset_subtype, txt, sizeof(last_preset_subtype) - 1);
					last_preset_subtype[sizeof(last_preset_subtype) - 1] = '\0';

					lv_obj_align_to(preset_pill, selected, LV_ALIGN_RIGHT_MID, -40, 0);
					lv_obj_remove_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);

					LV_LOG_WARN("No more subtypes; %d", current_selected_preset.level);

					lv_obj_add_flag(list_bg, LV_OBJ_FLAG_HIDDEN);
					lv_obj_add_flag(list_cont, LV_OBJ_FLAG_HIDDEN);
					lv_obj_add_flag(preset_pill, LV_OBJ_FLAG_USER_1);
					
				}
			}
		}
	}

	if (event_code == LV_EVENT_SCROLL_BEGIN)
	{
		void *param = lv_event_get_param(e);
		// LV_LOG_WARN("SCROLL BEGIN EVENT, param %d", param == NULL);
		lv_obj_add_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);
	}

	if (event_code == LV_EVENT_SCROLL_END)
	{
		void *param = lv_event_get_param(e);
		if (!lv_obj_has_flag(list_cont, LV_OBJ_FLAG_HIDDEN)){
			/* Scroll snapped */
			int32_t ps = lv_obj_get_scroll_y(list_cont);
			if (param == NULL){
				/* Scroll ended, snap position was adjusted */
				check_show_pill();
			} else {
				/* User scroll ended */
				int32_t pos = lv_obj_get_scroll_y(list_cont);
				int32_t index = (pos + 182) / 84;
				if ((pos + 182) % 84 == 0) {
					/* Scroll ended at exact item snap position */
					check_show_pill();
				}
			}
		}
	}

	if (event_code == LV_EVENT_DEFOCUSED) {


		// for (uint32_t i = 0; i < lv_obj_get_child_count(selected) - 1; i++) {
		// 	lv_obj_remove_flag(lv_obj_get_child(selected, i), LV_OBJ_FLAG_HIDDEN);
		// }

		// lv_obj_add_flag(list_bg, LV_OBJ_FLAG_HIDDEN);
		// lv_obj_add_flag(list_cont, LV_OBJ_FLAG_HIDDEN);

		// list_populate();
	}
	
}

static void check_show_pill()
{
	int32_t pos = lv_obj_get_scroll_y(list_cont);
	int32_t index = (pos + 182) / 84;

	lv_obj_add_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);
	if (current_selected_preset.level == 0){
		if (index < get_number_of_preset_profiles_cb()){
			// inbuilt presets may have subtypes
			cure_preset_t current = {0};
			get_cure_profile_info_from_index_cb(index, &current);
			if (current.subtype_size > 0){
				// subtypes are present
				if (current.subtype[0].subtype_size > 0){
					ui_shorten_string(current.subtype[0].subtype[0].name, type_output, sizeof(type_output));
					lv_label_set_text_fmt(preset_pill_text, "%s • %s", current.subtype[0].name, type_output);
					// set_cure_preset(current.subtype[0].subtype[0]);
				} else {
					lv_label_set_text_fmt(preset_pill_text, "%s", current.subtype[0].name);
					// set_cure_preset(current.subtype[0]);
				}
				lv_obj_align_to(preset_pill, selected, LV_ALIGN_RIGHT_MID, -40, 0);
				lv_obj_remove_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);
			} else {
				// set_cure_preset(current);
			}
		} else {
			// check custom presets
			bool is_create = lv_obj_has_flag(lv_obj_get_child(lv_obj_get_child(list_cont, index), 2), LV_OBJ_FLAG_HIDDEN);
			if (!is_create){
				// set_cure_preset(custom_presets[index - 4]);
			}
		}
		lv_obj_add_flag(title_bar, LV_OBJ_FLAG_HIDDEN);
	} else if (current_selected_preset.level == 1){
		printf("index %d\n", index);
		if (index > current_selected_preset.main->subtype_size - 1){
			printf("index out of bounds %d > %zu\n", index, current_selected_preset.main->subtype_size);
			return;
		}
		lv_label_set_text(title_bar_text, current_selected_preset.main->name);
		lv_obj_remove_flag(title_bar, LV_OBJ_FLAG_HIDDEN);

		if (current_selected_preset.main->subtype[index].subtype_size > 0){
			// subtypes are present
			printf("subnames present %zu\n", current_selected_preset.main->subtype[index].subtype_size);
			ui_shorten_string(current_selected_preset.main->subtype[index].subtype[0].name, type_output, sizeof(type_output));
			lv_label_set_text_fmt(preset_pill_text, "%s", type_output);
			printf("pill text %s\n", lv_label_get_text(preset_pill_text));
			lv_obj_align_to(preset_pill, selected, LV_ALIGN_RIGHT_MID, -40, 0);
			lv_obj_remove_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);
			// set_cure_preset(current_selected_preset.main->subtype[index].subtype[0]);
		} else {
			//
			// set_cure_preset(current_selected_preset.main->subtype[index]);
		}
	} else if (current_selected_preset.level == 2){
		printf("current_selected_preset level 2\n");
		lv_label_set_text_fmt(title_bar_text, "%s %s",  current_selected_preset.main->name, current_selected_preset.version->name);
		lv_obj_remove_flag(title_bar, LV_OBJ_FLAG_HIDDEN);
		// set_cure_preset(current_selected_preset.version->subtype[index]);
	}

	if (edit_mode || create_mode){
		edit_mode = false;
		create_mode = false;
		if (index < get_number_of_preset_profiles_cb())
		{
			ui_footer_toggle_buttons(footer, FT_SETTINGS | FT_CURE | FT_START);
		} else {
			ui_footer_toggle_buttons(footer, FT_SETTINGS | FT_EDIT | FT_CURE | FT_START);
		}
		
		lv_obj_add_flag(title_bar, LV_OBJ_FLAG_HIDDEN);
	}

	printf("exit check_show_pill\n");
}

static void ui_switch_clicked_event(lv_event_t *e)
{
	if (!create_mode && !edit_mode){

		for (uint32_t i = 0; i < lv_obj_get_child_count(selected) - 1; i++) {
			lv_obj_remove_flag(lv_obj_get_child(selected, i), LV_OBJ_FLAG_HIDDEN);
		}

		lv_obj_add_flag(list_bg, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(list_cont, LV_OBJ_FLAG_HIDDEN);

		update_selected_item(0, "No preset selected", true);
		lv_obj_add_flag(preset_pill, LV_OBJ_FLAG_HIDDEN);
		lv_obj_remove_flag(preset_pill, LV_OBJ_FLAG_USER_1);
		ui_footer_toggle_buttons(footer, FT_SETTINGS | FT_CURE | FT_START);
		// lv_obj_scroll_to_view(lv_obj_get_child(list_cont, 0), LV_ANIM_OFF);

		// list_populate();
	}

}

static void heat_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	LV_UNUSED(observer);

	int32_t v = lv_subject_get_int(subject);

	obj_modify_state_recursive(pre_cont, LV_STATE_DISABLED, v, 2);
	
	if (v){
		if (lv_subject_get_int(&subject_switch_cure) && !lv_subject_get_int(&subject_time_cure)){
			lv_obj_add_state(footer->ft_start, LV_STATE_DISABLED);
		} else {
			lv_obj_remove_state(footer->ft_start, LV_STATE_DISABLED);
		}
	} else {
		if (lv_subject_get_int(&subject_switch_cure) && lv_subject_get_int(&subject_time_cure)){
			lv_obj_remove_state(footer->ft_start, LV_STATE_DISABLED);
		} else {
			lv_obj_add_state(footer->ft_start, LV_STATE_DISABLED);
		}
	}
	
	
}

static void cure_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	LV_UNUSED(observer);

	int32_t v = lv_subject_get_int(subject);

	obj_modify_state_recursive(cure_cont, LV_STATE_DISABLED, v, 2);

	// lv_obj_remove_state(cure_switch, LV_STATE_DISABLED);

	if (lv_subject_get_int(v ? &subject_time_cure : &subject_switch_heat)) {
		lv_obj_remove_state(footer->ft_start, LV_STATE_DISABLED);
	} else {
		lv_obj_add_state(footer->ft_start, LV_STATE_DISABLED);
	}
	
}

static void update_switch(lv_obj_t * switch_obj, bool state, bool send_event)
{
	
	if (state){
		lv_obj_add_state(switch_obj, LV_STATE_CHECKED);
	} else {
		lv_obj_remove_state(switch_obj, LV_STATE_CHECKED);
	}

	if (send_event){
		lv_obj_send_event(switch_obj, LV_EVENT_VALUE_CHANGED, NULL);
	}
	
}

static void cure_time_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	LV_UNUSED(observer);

	int32_t v = lv_subject_get_int(subject);
	if (v > 0)
	{
		lv_obj_remove_state(footer->ft_start, LV_STATE_DISABLED);
	}
	else 
	{
		lv_obj_add_state(footer->ft_start, LV_STATE_DISABLED);
	}
}


static void cure_icon_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	LV_UNUSED(observer);
	int32_t v = lv_subject_get_int(subject);

	const lv_image_dsc_t * src = cure_preset_icons[LV_CLAMP(0, v, 5)];

	lv_image_set_src(lv_obj_get_child(selected, 0), src);
	if (src == NULL){
		lv_obj_set_width(lv_obj_get_child(selected, 0), 0);
	} else {
		lv_obj_set_width(lv_obj_get_child(selected, 0), LV_SIZE_CONTENT);
	}
}
