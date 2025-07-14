/**
 * @file ui_screen_heat_cool.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../ui.h"
#include "ui_screens.h"
#include "../components/ui_footer.h"
#include "../components/ui_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void ui_footer_events(lv_event_t *e);
static void ui_screen_heat_cool_events(lv_event_t *e);

static void cure_icon_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void cure_temperature_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
// static void heat_temperature_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void temp_anim_exec_cb(lv_anim_t* a, int32_t v);
static void temp_anim_complete_cb(lv_anim_t *a);
static void temperature_anim_start(uint32_t start, uint32_t end);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * heat_cool_screen;
static lv_obj_t * main_cont;
static lv_obj_t * title_cont;
static lv_obj_t * title_label;
static lv_obj_t * image;
static lv_obj_t * preset_icon;
static lv_obj_t * cure_temp_label;
// static lv_obj_t * heat_temp_label;
static lv_obj_t * target_temp_label;

static ui_footer_t * footer;

static lv_anim_t temperature_anim;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_screen_heat_cool_init(void)
{
	heat_cool_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(heat_cool_screen, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(heat_cool_screen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(heat_cool_screen, LV_OBJ_FLAG_SCROLLABLE);


	main_cont = lv_obj_create(heat_cool_screen);
	lv_obj_remove_style_all(main_cont);
	lv_obj_set_width(main_cont, 950);
	lv_obj_set_height(main_cont, 261);
	lv_obj_set_x(main_cont, 60);
	lv_obj_set_y(main_cont, 60);
	lv_obj_set_align(main_cont, LV_ALIGN_TOP_LEFT);
	lv_obj_set_flex_flow(main_cont,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(main_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_color(main_cont, COLOR_INTERACTIVE_SECONDARY, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(main_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);


	title_cont = lv_obj_create(main_cont);
	lv_obj_remove_style_all(title_cont);
	lv_obj_set_width(title_cont, 950);
	lv_obj_set_height(title_cont, 60);
	lv_obj_set_align(title_cont, LV_ALIGN_TOP_LEFT);
	lv_obj_set_flex_flow(title_cont,LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(title_cont,  LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(title_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_column(title_cont, 10, 0);
	lv_obj_set_style_bg_color(title_cont, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(title_cont, 0, LV_PART_MAIN| LV_STATE_DEFAULT);



	LV_FONT_DECLARE(font_proxima_28)
	LV_FONT_DECLARE(font_proxima_bold_44)

	
	title_label = ui_label_create(title_cont, "Heating", LV_SIZE_CONTENT, 60);
	lv_obj_set_style_text_font(title_label, &font_proxima_bold_44, 0);
	lv_obj_set_style_text_color(title_label, COLOR_TEXT_PRIMARY, 0);

	lv_obj_t * label;
	label = ui_label_create(title_cont, "32 °C", 150, 60);
	lv_label_bind_text(label, &subject_current_temperature, "%d °C");
	lv_obj_set_style_text_font(label, &font_proxima_bold_44, 0);
	lv_obj_set_style_text_color(label, COLOR_TEXT_PRIMARY, 0);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);

	LV_IMAGE_DECLARE(image_next_icon);
	lv_obj_t * img = lv_image_create(title_cont);
	lv_image_set_src(img, &image_next_icon);

	target_temp_label = ui_label_create(title_cont, "60 °C", 150, 60);
	// lv_label_bind_text(label, &subject_temperature_heat, "%d °C");
	lv_obj_set_style_text_font(target_temp_label, &font_proxima_bold_44, 0);
	lv_obj_set_style_text_color(target_temp_label, COLOR_TEXT_PRIMARY, 0);
	lv_obj_set_style_text_align(target_temp_label, LV_TEXT_ALIGN_RIGHT, 0);

	
	title_cont = lv_obj_create(main_cont);
	lv_obj_remove_style_all(title_cont);
	lv_obj_set_width(title_cont, LV_SIZE_CONTENT);
	lv_obj_set_height(title_cont, 52);
	lv_obj_set_align(title_cont, LV_ALIGN_TOP_LEFT);
	lv_obj_set_flex_flow(title_cont,LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(title_cont,  LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(title_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_column(title_cont, 10, 0);
	lv_obj_set_style_pad_top(title_cont, 6, 0);
	lv_obj_set_style_bg_color(title_cont, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(title_cont, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

	LV_IMAGE_DECLARE(image_droplet_white_icon);
	preset_icon = lv_image_create(title_cont);
	lv_image_set_src(preset_icon, &image_droplet_white_icon);

	label = ui_label_create(title_cont, "White v5", LV_SIZE_CONTENT, 40);
	lv_label_bind_text(label, &subject_preset_type_text, NULL);
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);
	lv_obj_set_style_text_color(label, COLOR_INTERACTIVE_DISABLED, 0);


	title_cont = lv_obj_create(main_cont);
	lv_obj_remove_style_all(title_cont);
	lv_obj_set_width(title_cont, LV_SIZE_CONTENT);
	lv_obj_set_height(title_cont, 52);
	lv_obj_set_align(title_cont, LV_ALIGN_TOP_LEFT);
	lv_obj_set_flex_flow(title_cont,LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(title_cont,  LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(title_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_column(title_cont, 10, 0);
	lv_obj_set_style_pad_top(title_cont, 6, 0);
	lv_obj_set_style_bg_color(title_cont, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(title_cont, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

	LV_IMAGE_DECLARE(image_pre_heat_icon);
	img = lv_image_create(title_cont);
	lv_image_set_src(img, &image_pre_heat_icon);
	lv_obj_set_style_image_recolor(img, COLOR_INTERACTIVE_DISABLED, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_image_recolor_opa(img, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

	label = ui_label_create(title_cont, "Pre-heat:", LV_SIZE_CONTENT, 40);
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);
	lv_obj_set_style_text_color(label, COLOR_INTERACTIVE_DISABLED, 0);

	label = ui_label_create(title_cont, "10 min", LV_SIZE_CONTENT, 40);
	lv_label_bind_text(label, &subject_time_heat, "%d min");
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);
	lv_obj_set_style_text_color(label, COLOR_INTERACTIVE_DISABLED, 0);

	// label = ui_label_create(title_cont, "•", LV_SIZE_CONTENT, 40);
	// lv_obj_set_style_text_font(label, &font_proxima_28, 0);
	// lv_obj_set_style_text_color(label, COLOR_INTERACTIVE_DISABLED, 0);

	// heat_temp_label = ui_label_create(title_cont, "60 °C", LV_SIZE_CONTENT, 40);
	// lv_label_bind_text(heat_temp_label, &subject_temperature_heat, "%d °C");
	// lv_obj_set_style_text_font(heat_temp_label, &font_proxima_28, 0);
	// lv_obj_set_style_text_color(heat_temp_label, COLOR_INTERACTIVE_DISABLED, 0);


	title_cont = lv_obj_create(main_cont);
	lv_obj_remove_style_all(title_cont);
	lv_obj_set_width(title_cont, LV_SIZE_CONTENT);
	lv_obj_set_height(title_cont, 52);
	lv_obj_set_align(title_cont, LV_ALIGN_TOP_LEFT);
	lv_obj_set_flex_flow(title_cont,LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(title_cont,  LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(title_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_column(title_cont, 10, 0);
	lv_obj_set_style_pad_top(title_cont, 6, 0);
	lv_obj_set_style_bg_color(title_cont, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(title_cont, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

	LV_IMAGE_DECLARE(image_curing_icon);
	img = lv_image_create(title_cont);
	lv_image_set_src(img, &image_curing_icon);
	lv_obj_set_style_image_recolor(img, COLOR_INTERACTIVE_DISABLED, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_image_recolor_opa(img, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

	label = ui_label_create(title_cont, "Cure:", LV_SIZE_CONTENT, 40);
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);
	lv_obj_set_style_text_color(label, COLOR_INTERACTIVE_DISABLED, 0);

	label = ui_label_create(title_cont, "10 min", LV_SIZE_CONTENT, 40);
	lv_label_bind_text(label, &subject_time_cure, "%d min");
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);
	lv_obj_set_style_text_color(label, COLOR_INTERACTIVE_DISABLED, 0);

	label = ui_label_create(title_cont, "•", LV_SIZE_CONTENT, 40);
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);
	lv_obj_set_style_text_color(label, COLOR_INTERACTIVE_DISABLED, 0);

	cure_temp_label = ui_label_create(title_cont, "60 °C", LV_SIZE_CONTENT, 40);
	lv_label_bind_text(cure_temp_label, &subject_temperature_cure, "%d °C");
	lv_obj_set_style_text_font(cure_temp_label, &font_proxima_28, 0);
	lv_obj_set_style_text_color(cure_temp_label, COLOR_INTERACTIVE_DISABLED, 0);


	/* Footer object */
	footer = ui_footer_create(heat_cool_screen, FT_ABORT, ui_footer_events);


	LV_IMAGE_DECLARE(image_heating);
	image = lv_image_create(heat_cool_screen);
	lv_image_set_src(image, &image_heating);
	lv_obj_set_width(image, LV_SIZE_CONTENT);
	lv_obj_set_height(image, LV_SIZE_CONTENT); 
	lv_obj_set_x(image, 930);
	lv_obj_set_y(image, 50);
	lv_obj_set_align(image, LV_ALIGN_TOP_LEFT);
	lv_obj_add_flag(image, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_remove_flag(image, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_add_event_cb(heat_cool_screen, ui_screen_heat_cool_events, LV_EVENT_ALL, NULL);

	lv_subject_add_observer(&subject_preset_icon, cure_icon_observer_cb, NULL);
	lv_subject_add_observer(&subject_temperature_cure, cure_temperature_observer_cb, NULL);
	// lv_subject_add_observer(&subject_temperature_heat, heat_temperature_observer_cb, NULL);

}

lv_obj_t * ui_screen_heat_cool(void)
{
	if (!heat_cool_screen){
		ui_screen_heat_cool_init();
	}
	return heat_cool_screen;
}

ui_footer_t *ui_footer_heat_cool(void)
{
	if (!heat_cool_screen){
		ui_screen_heat_cool_init();
	}
	return footer;
}

void show_heat_cool_screen(void)
{
	lv_disp_load_scr(heat_cool_screen);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ui_footer_events(lv_event_t *e)
{
	footer_button_t button = (footer_button_t)(intptr_t)lv_event_get_user_data(e);
	/* Handle button pressed actions from the callback */
	on_footer_button_pressed_cb(SCREEN_HEAT_COOL, button);

	/* Handle button pressed actions in this file */
	switch (button){
		case FT_ABORT:
			show_finished_screen(true);
		// lv_screen_load(ui_screen_finished);
		break;
		default:
			break;
	}

}

static void ui_screen_heat_cool_events(lv_event_t *e)
{
	lv_event_code_t event_code = lv_event_get_code(e);

	if (event_code == LV_EVENT_SCREEN_LOADED){
		on_screen_loaded_cb(SCREEN_HEAT_COOL);

		lv_label_set_text_fmt(target_temp_label, "%d °C", lv_subject_get_int(&subject_temperature_cure));

#if !LV_USE_SDL
		LV_IMAGE_DECLARE(image_heating);
		lv_label_set_text(title_label, "Heating");
		lv_image_set_src(image, &image_heating);

#else
		lv_subject_set_int(&subject_current_temperature, lv_rand(lv_subject_get_int(&subject_temperature_heat) - 20, lv_subject_get_int(&subject_temperature_heat) + 20));

		if (lv_subject_get_int(&subject_current_temperature) > lv_subject_get_int(&subject_temperature_heat)){
			LV_IMAGE_DECLARE(image_cooling);
			lv_label_set_text(title_label, "Cooling");
			lv_image_set_src(image, &image_cooling);
		} else {
			LV_IMAGE_DECLARE(image_heating);
			lv_label_set_text(title_label, "Heating");
			lv_image_set_src(image, &image_heating);
		}

		temperature_anim_start(lv_subject_get_int(&subject_current_temperature), lv_subject_get_int(&subject_temperature_heat));
#endif
	}

	if (event_code == LV_EVENT_SCREEN_UNLOAD_START){

		bool state = lv_anim_custom_delete(&temperature_anim, NULL);

	}
}

static void cure_icon_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	LV_UNUSED(observer);
	int32_t v = lv_subject_get_int(subject);

	const lv_image_dsc_t * src = cure_preset_icons[LV_CLAMP(0, v, 5)];

	lv_image_set_src(preset_icon, src);
	if (src == NULL){
		lv_obj_set_width(preset_icon, 0);
	} else {
		lv_obj_set_width(preset_icon, LV_SIZE_CONTENT);
	}
}

static void temp_anim_exec_cb(lv_anim_t* a, int32_t v)
{
	lv_subject_set_int(&subject_current_temperature, v);
}

static void temp_anim_complete_cb(lv_anim_t *a)
{
#if LV_USE_SDL
	lv_screen_load(ui_screen_curing());
#endif
}

static void temperature_anim_start(uint32_t start, uint32_t end)
{
    lv_anim_init(&temperature_anim);
    lv_anim_set_values(&temperature_anim, start, end);
    lv_anim_set_duration(&temperature_anim, 3000);
	lv_anim_set_custom_exec_cb(&temperature_anim, temp_anim_exec_cb);
	lv_anim_set_completed_cb(&temperature_anim, temp_anim_complete_cb);
    lv_anim_start(&temperature_anim);
}

// static void heat_temperature_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
// {
// 	LV_UNUSED(observer);

// 	int32_t v = lv_subject_get_int(subject);
// 	if (v == 34)
// 	{
// 		lv_label_set_text(heat_temp_label, "-- °C");
// 	}
// }

static void cure_temperature_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	LV_UNUSED(observer);

	int32_t v = lv_subject_get_int(subject);
	if (v < 35)
	{
		lv_label_set_text(cure_temp_label, "-- °C");
	}
}
