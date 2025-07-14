/**
 * @file ui_screen_curing.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../ui.h"
#include "../screens/ui_screens.h"
#include "../components/ui_button.h"
#include "../components/ui_footer.h"
#include "../components/ui_label.h"
#include "../components/ui_list_element.h"
#include "../components/ui_spinbox.h"

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

static void ui_screen_cure_events(lv_event_t *e);

static void cure_icon_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void cure_temperature_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
// static void heat_temperature_observer_cb(lv_observer_t *observer, lv_subject_t *subject);


/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *curing_screen;

static ui_footer_t *footer;
static lv_obj_t *preset_icon;

static lv_obj_t *info_cont;
static lv_obj_t *footer_button;
static lv_obj_t *progress_bar_heat;
static lv_obj_t *progress_bar_cure;

static lv_obj_t *title_cont;
static lv_obj_t *heat_cont;
static lv_obj_t *cure_cont;

static lv_obj_t *heat_bar_icon;
static lv_obj_t *cure_bar_icon;
static lv_obj_t *finished_bar_icon;
static lv_obj_t *skipped_label;
static lv_obj_t *paused_cont;
static lv_obj_t *rem_label;
static lv_obj_t *cure_temp_label;
static lv_obj_t *hours_label;
static lv_obj_t *time_label;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_screen_curing_init(void)
{

	curing_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(curing_screen, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(curing_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_remove_flag(curing_screen, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t *progress_cont = lv_obj_create(curing_screen);
	lv_obj_remove_style_all(progress_cont);
	lv_obj_set_width(progress_cont, 1240);
	lv_obj_set_height(progress_cont, 60);
	lv_obj_set_x(progress_cont, 20);
	lv_obj_set_y(progress_cont, 20);
	lv_obj_remove_flag(progress_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

	progress_bar_heat = lv_bar_create(progress_cont);
	lv_bar_set_range(progress_bar_heat, 0, PROGRESS_BAR_MAX);
	lv_bar_set_value(progress_bar_heat, 25, LV_ANIM_OFF);
	lv_bar_set_start_value(progress_bar_heat, 0, LV_ANIM_OFF);
	lv_obj_set_width(progress_bar_heat, 590);
	lv_obj_set_height(progress_bar_heat, 10);
	lv_obj_set_x(progress_bar_heat, 30);
	lv_obj_set_align(progress_bar_heat, LV_ALIGN_LEFT_MID);
	lv_obj_set_style_radius(progress_bar_heat, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(progress_bar_heat, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(progress_bar_heat, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	lv_obj_set_style_radius(progress_bar_heat, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(progress_bar_heat, COLOR_INTERACTIVE_ACTIVE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(progress_bar_heat, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

	progress_bar_cure = lv_bar_create(progress_cont);
	lv_bar_set_range(progress_bar_cure, 0, PROGRESS_BAR_MAX);
	lv_bar_set_value(progress_bar_cure, 25, LV_ANIM_OFF);
	lv_bar_set_start_value(progress_bar_cure, 0, LV_ANIM_OFF);
	lv_obj_set_width(progress_bar_cure, 590);
	lv_obj_set_height(progress_bar_cure, 10);
	lv_obj_set_x(progress_bar_cure, -30);
	lv_obj_set_align(progress_bar_cure, LV_ALIGN_RIGHT_MID);
	lv_obj_set_style_radius(progress_bar_cure, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(progress_bar_cure, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(progress_bar_cure, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	lv_obj_set_style_radius(progress_bar_cure, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(progress_bar_cure, COLOR_INTERACTIVE_ACTIVE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(progress_bar_cure, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

	lv_obj_t *bar_icon_cont = lv_obj_create(progress_cont);
	lv_obj_remove_style_all(bar_icon_cont);
	lv_obj_set_width(bar_icon_cont, 1240);
	lv_obj_set_height(bar_icon_cont, 60);
	lv_obj_set_align(bar_icon_cont, LV_ALIGN_TOP_MID);
	lv_obj_remove_flag(bar_icon_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

	LV_IMAGE_DECLARE(image_heat_icon);
	heat_bar_icon = lv_image_create(bar_icon_cont);
	lv_image_set_src(heat_bar_icon, &image_heat_icon);
	lv_obj_set_width(heat_bar_icon, 60);
	lv_obj_set_height(heat_bar_icon, 60);
	lv_obj_set_align(heat_bar_icon, LV_ALIGN_LEFT_MID);
	lv_obj_add_state(heat_bar_icon, LV_STATE_CHECKED);
	lv_obj_add_flag(heat_bar_icon, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_remove_flag(heat_bar_icon, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_radius(heat_bar_icon, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(heat_bar_icon, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(heat_bar_icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(heat_bar_icon, COLOR_INTERACTIVE_ACTIVE, LV_PART_MAIN | LV_STATE_CHECKED);
	lv_obj_set_style_bg_opa(heat_bar_icon, 255, LV_PART_MAIN | LV_STATE_CHECKED);

	LV_IMAGE_DECLARE(image_cure_icon);
	cure_bar_icon = lv_image_create(bar_icon_cont);
	lv_image_set_src(cure_bar_icon, &image_cure_icon);
	lv_obj_set_width(cure_bar_icon, 60);
	lv_obj_set_height(cure_bar_icon, 60);
	lv_obj_set_align(cure_bar_icon, LV_ALIGN_LEFT_MID);
	lv_obj_add_flag(cure_bar_icon, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_remove_flag(cure_bar_icon, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_radius(cure_bar_icon, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(cure_bar_icon, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(cure_bar_icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(cure_bar_icon, COLOR_INTERACTIVE_ACTIVE, LV_PART_MAIN | LV_STATE_CHECKED);
	lv_obj_set_style_bg_opa(cure_bar_icon, 255, LV_PART_MAIN | LV_STATE_CHECKED);

	LV_IMAGE_DECLARE(image_finished_icon);
	finished_bar_icon = lv_image_create(bar_icon_cont);
	lv_image_set_src(finished_bar_icon, &image_finished_icon);
	lv_obj_set_width(finished_bar_icon, 60);
	lv_obj_set_height(finished_bar_icon, 60);
	lv_obj_set_align(finished_bar_icon, LV_ALIGN_RIGHT_MID);
	lv_obj_add_flag(finished_bar_icon, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_remove_flag(finished_bar_icon, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_radius(finished_bar_icon, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(finished_bar_icon, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(finished_bar_icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(finished_bar_icon, COLOR_INTERACTIVE_ACTIVE, LV_PART_MAIN | LV_STATE_CHECKED);
	lv_obj_set_style_bg_opa(finished_bar_icon, 255, LV_PART_MAIN | LV_STATE_CHECKED);

	lv_obj_t *main_cont = lv_obj_create(curing_screen);
	lv_obj_remove_style_all(main_cont);
	lv_obj_set_width(main_cont, 1230);
	lv_obj_set_height(main_cont, 208);
	lv_obj_set_x(main_cont, 20);
	lv_obj_set_y(main_cont, 95);
	lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);


	lv_obj_t *time_cont = lv_obj_create(main_cont);
	lv_obj_remove_style_all(time_cont);
	lv_obj_set_width(time_cont, 585);
	lv_obj_set_height(time_cont, 153);
	lv_obj_set_flex_flow(time_cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(time_cont, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	LV_FONT_DECLARE(font_proxima_bold_128);
	hours_label = ui_label_create(time_cont, "13:", LV_SIZE_CONTENT, 153);
	lv_obj_set_style_text_font(hours_label, &font_proxima_bold_128, 0);
	lv_obj_set_style_text_color(hours_label, COLOR_TEXT_PRIMARY, 0);
	lv_obj_set_style_text_align(hours_label, LV_TEXT_ALIGN_RIGHT, 0);

	time_label = ui_label_create(time_cont, "45", 170, 153);
	lv_obj_set_style_text_font(time_label, &font_proxima_bold_128, 0);
	lv_obj_set_style_text_color(time_label, COLOR_TEXT_PRIMARY, 0);
	lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_LEFT, 0);

	lv_obj_t *info_cont = lv_obj_create(main_cont);
	lv_obj_remove_style_all(info_cont);
	lv_obj_set_width(info_cont, 585);
	lv_obj_set_height(info_cont, lv_pct(100));
	lv_obj_set_align(info_cont, LV_ALIGN_CENTER);
	lv_obj_set_flex_flow(info_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(info_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

	paused_cont = lv_obj_create(info_cont);
	lv_obj_remove_style_all(paused_cont);
	lv_obj_set_width(paused_cont, LV_SIZE_CONTENT);
	lv_obj_set_height(paused_cont, 52);
	lv_obj_set_align(paused_cont, LV_ALIGN_TOP_LEFT);
	lv_obj_set_flex_flow(paused_cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(paused_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(paused_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_column(paused_cont, 10, 0);
	lv_obj_set_style_pad_top(paused_cont, 6, 0);
	lv_obj_set_style_bg_color(paused_cont, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(paused_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

	LV_IMAGE_DECLARE(image_time_icon);
	LV_FONT_DECLARE(font_proxima_28);
	lv_obj_t *img;
	img = lv_image_create(paused_cont);
	lv_image_set_src(img, &image_time_icon);
	lv_obj_set_style_image_recolor(img, COLOR_INTERACTIVE_DISABLED, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_set_style_image_recolor_opa(img, 255, LV_PART_MAIN | LV_STATE_DISABLED);

	rem_label = ui_label_create(paused_cont, "Remaining: 13:45", LV_SIZE_CONTENT, 40);
	lv_obj_set_style_text_font(rem_label, &font_proxima_28, 0);
	lv_obj_add_flag(paused_cont, LV_OBJ_FLAG_HIDDEN);

	title_cont = lv_obj_create(info_cont);
	lv_obj_remove_style_all(title_cont);
	lv_obj_set_width(title_cont, LV_SIZE_CONTENT);
	lv_obj_set_height(title_cont, 52);
	lv_obj_set_align(title_cont, LV_ALIGN_TOP_LEFT);
	lv_obj_set_flex_flow(title_cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(title_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(title_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_column(title_cont, 10, 0);
	lv_obj_set_style_pad_top(title_cont, 6, 0);
	lv_obj_set_style_bg_color(title_cont, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(title_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

	LV_IMAGE_DECLARE(image_droplet_white_icon);
	preset_icon = lv_image_create(title_cont);
	lv_image_set_src(preset_icon, &image_droplet_white_icon);

	lv_obj_t *label;

	label = ui_label_create(title_cont, "White V5", LV_SIZE_CONTENT, 40);
	lv_label_bind_text(label, &subject_preset_type_text, NULL);
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);
	lv_obj_set_style_text_color(label, COLOR_INTERACTIVE_DISABLED, 0);

	heat_cont = lv_obj_create(info_cont);
	lv_obj_remove_style_all(heat_cont);
	lv_obj_set_width(heat_cont, LV_SIZE_CONTENT);
	lv_obj_set_height(heat_cont, 52);
	lv_obj_set_align(heat_cont, LV_ALIGN_TOP_LEFT);
	lv_obj_set_flex_flow(heat_cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(heat_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(heat_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_column(heat_cont, 10, 0);
	lv_obj_set_style_pad_top(heat_cont, 6, 0);
	lv_obj_set_style_bg_color(heat_cont, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(heat_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

	LV_IMAGE_DECLARE(image_pre_heat_icon);
	img = lv_image_create(heat_cont);
	lv_image_set_src(img, &image_pre_heat_icon);
	lv_obj_set_style_image_recolor(img, COLOR_INTERACTIVE_DISABLED, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_set_style_image_recolor_opa(img, 255, LV_PART_MAIN | LV_STATE_DISABLED);

	label = ui_label_create(heat_cont, "Pre-heat:", LV_SIZE_CONTENT, 40);
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);

	label = ui_label_create(heat_cont, "10 min", LV_SIZE_CONTENT, 40);
	lv_label_bind_text(label, &subject_time_heat, "%d min");
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);

	// label = ui_label_create(heat_cont, "•", LV_SIZE_CONTENT, 40);
	// lv_obj_set_style_text_font(label, &font_proxima_28, 0);

	// heat_temp_label = ui_label_create(heat_cont, "60 °C", LV_SIZE_CONTENT, 40);
	// lv_label_bind_text(heat_temp_label, &subject_temperature_heat, "%d °C");
	// lv_obj_set_style_text_font(heat_temp_label, &font_proxima_28, 0);

	skipped_label = ui_label_create(heat_cont, "(Skipped)", LV_SIZE_CONTENT, 40);
	lv_obj_set_style_text_font(skipped_label, &font_proxima_28, 0);
	lv_obj_add_flag(skipped_label, LV_OBJ_FLAG_HIDDEN);

	obj_add_state_recursive(heat_cont, LV_STATE_DISABLED, 2);

	cure_cont = lv_obj_create(info_cont);
	lv_obj_remove_style_all(cure_cont);
	lv_obj_set_width(cure_cont, LV_SIZE_CONTENT);
	lv_obj_set_height(cure_cont, 52);
	lv_obj_set_align(cure_cont, LV_ALIGN_TOP_LEFT);
	lv_obj_set_flex_flow(cure_cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(cure_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(cure_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_column(cure_cont, 10, 0);
	lv_obj_set_style_pad_top(cure_cont, 6, 0);
	lv_obj_set_style_bg_color(cure_cont, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(cure_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

	LV_IMAGE_DECLARE(image_curing_icon);
	img = lv_image_create(cure_cont);
	lv_image_set_src(img, &image_curing_icon);
	lv_obj_set_style_image_recolor(img, COLOR_INTERACTIVE_DISABLED, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_set_style_image_recolor_opa(img, 255, LV_PART_MAIN | LV_STATE_DISABLED);

	label = ui_label_create(cure_cont, "Cure:", LV_SIZE_CONTENT, 40);
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);

	label = ui_label_create(cure_cont, "10 min", LV_SIZE_CONTENT, 40);
	lv_label_bind_text(label, &subject_time_cure, "%d min");
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);

	label = ui_label_create(cure_cont, "•", LV_SIZE_CONTENT, 40);
	lv_obj_set_style_text_font(label, &font_proxima_28, 0);

	cure_temp_label = ui_label_create(cure_cont, "60 °C", LV_SIZE_CONTENT, 40);
	lv_label_bind_text(cure_temp_label, &subject_temperature_cure, "%d °C");
	lv_obj_set_style_text_font(cure_temp_label, &font_proxima_28, 0);

	obj_add_state_recursive(cure_cont, LV_STATE_DISABLED, 2);

	/* Footer object */
	footer = ui_footer_create(curing_screen, FT_ABORT | FT_PAUSE, ui_footer_events);

	lv_subject_add_observer(&subject_preset_icon, cure_icon_observer_cb, NULL);
	lv_subject_add_observer(&subject_temperature_cure, cure_temperature_observer_cb, NULL);
	// lv_subject_add_observer(&subject_temperature_heat, heat_temperature_observer_cb, NULL);
	
	lv_obj_add_event_cb(curing_screen, ui_screen_cure_events, LV_EVENT_ALL, NULL);
}

lv_obj_t *ui_screen_curing(void)
{
	if (!curing_screen){
		ui_screen_curing_init();
	}
	return curing_screen;
}

ui_footer_t *ui_footer_curing(void)
{
	if (!curing_screen){
		ui_screen_curing_init();
	}
	return footer;
}


void progress_bar_update(int32_t pre_heat_progress, int32_t cure_progress, bool skip_heat, bool paused)
{

	if (skip_heat){
		lv_obj_remove_flag(skipped_label, LV_OBJ_FLAG_HIDDEN);
	} else {
		lv_obj_add_flag(skipped_label, LV_OBJ_FLAG_HIDDEN);
	}

	if (paused){
		lv_obj_remove_flag(paused_cont, LV_OBJ_FLAG_HIDDEN);
		obj_add_state_recursive(cure_cont, LV_STATE_DISABLED, 2);
		obj_add_state_recursive(heat_cont, LV_STATE_DISABLED, 2);
	} else {
		lv_obj_add_flag(paused_cont, LV_OBJ_FLAG_HIDDEN);
	}

	footer_button_t type = paused ? FT_RESUME : FT_PAUSE;
	footer_button_t skip = paused ? FT_NONE : FT_NONE;

	lv_bar_set_value(progress_bar_heat, pre_heat_progress, LV_ANIM_OFF);
	lv_bar_set_value(progress_bar_cure, cure_progress, LV_ANIM_OFF);

	if (lv_subject_get_int(&subject_switch_cure) && lv_subject_get_int(&subject_time_heat))
	{
		/* Heat & Cure stages enabled */
		if (cure_progress == PROGRESS_BAR_MAX)
		{
			/* Heat & Cure complete*/
			lv_obj_add_state(finished_bar_icon, LV_STATE_CHECKED);
			lv_obj_add_state(cure_bar_icon, LV_STATE_CHECKED);

			obj_add_state_recursive(heat_cont, LV_STATE_DISABLED, 2);
			obj_add_state_recursive(cure_cont, LV_STATE_DISABLED, 2);

			ui_footer_toggle_buttons(footer, FT_DONE);
		}
		else if (pre_heat_progress == PROGRESS_BAR_MAX && cure_progress < PROGRESS_BAR_MAX)
		{
			/* Heating complete */
			lv_obj_add_state(cure_bar_icon, LV_STATE_CHECKED);
			lv_obj_remove_state(finished_bar_icon, LV_STATE_CHECKED);

			obj_add_state_recursive(heat_cont, LV_STATE_DISABLED, 2);
			obj_remove_state_recursive(cure_cont, LV_STATE_DISABLED, 2);

			ui_footer_toggle_buttons(footer, FT_ABORT | type);
		}
		else
		{
			/* No stage completed */
			lv_obj_remove_state(cure_bar_icon, LV_STATE_CHECKED);
			lv_obj_remove_state(finished_bar_icon, LV_STATE_CHECKED);

			obj_remove_state_recursive(heat_cont, LV_STATE_DISABLED, 2);
			obj_add_state_recursive(cure_cont, LV_STATE_DISABLED, 2);
			ui_footer_toggle_buttons(footer, FT_ABORT | skip | type);
		}

		

	} else if (lv_subject_get_int(&subject_time_heat))
	{
		/* Heat stage only */

		if (pre_heat_progress == PROGRESS_BAR_MAX) {
			/* Heating complete */
			lv_obj_add_state(heat_bar_icon, LV_STATE_CHECKED);
			lv_obj_add_state(finished_bar_icon, LV_STATE_CHECKED);

			obj_add_state_recursive(heat_cont, LV_STATE_DISABLED, 2);

			ui_footer_toggle_buttons(footer, FT_DONE);

		} else {

			lv_obj_add_state(heat_bar_icon, LV_STATE_CHECKED);
			lv_obj_remove_state(finished_bar_icon, LV_STATE_CHECKED);

			obj_remove_state_recursive(heat_cont, LV_STATE_DISABLED, 2);

			ui_footer_toggle_buttons(footer, FT_ABORT | type);
		}

		
	}
	else if (lv_subject_get_int(&subject_switch_cure))
	{
		/* Curing stage only */
		if (cure_progress == PROGRESS_BAR_MAX){

			lv_obj_add_state(cure_bar_icon, LV_STATE_CHECKED);
			lv_obj_add_state(finished_bar_icon, LV_STATE_CHECKED);

			obj_add_state_recursive(cure_cont, LV_STATE_DISABLED, 2);

			ui_footer_toggle_buttons(footer, FT_DONE);

		} else {
			lv_obj_add_state(cure_bar_icon, LV_STATE_CHECKED);
			lv_obj_remove_state(finished_bar_icon, LV_STATE_CHECKED);

			obj_remove_state_recursive(cure_cont, LV_STATE_DISABLED, 2);

			ui_footer_toggle_buttons(footer, FT_ABORT | type);
		}
		
	}
}

void set_cure_text(const char * text)
{
	if (!lv_obj_has_flag(lv_obj_get_parent(time_label), LV_OBJ_FLAG_HIDDEN)){
		lv_obj_add_flag(lv_obj_get_parent(time_label), LV_OBJ_FLAG_HIDDEN);
	}
	lv_label_set_text(hours_label, text);
}

void set_cure_time(int32_t hour, int32_t minute)
{
	if (lv_obj_has_flag(lv_obj_get_parent(time_label), LV_OBJ_FLAG_HIDDEN)){
		lv_obj_remove_flag(lv_obj_get_parent(time_label), LV_OBJ_FLAG_HIDDEN);
	}
	lv_label_set_text_fmt(hours_label, "%02d:", hour);
	lv_label_set_text_fmt(time_label, "%02d", minute);
}

void set_remaining_time(const char * text)
{
	lv_label_set_text(rem_label, text);
}

void show_curing_screen(void)
{
	lv_screen_load(curing_screen);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ui_footer_events(lv_event_t *e)
{
	footer_button_t button = (footer_button_t)(intptr_t)lv_event_get_user_data(e);

	/* Handle button pressed actions from the callback */
	on_footer_button_pressed_cb(SCREEN_CURE, button);

	/* Handle button pressed actions in this file */
	switch (button)
	{
	case FT_ABORT:
		
		show_finished_screen(true);
		// lv_screen_load(ui_screen_home());
		break;
	case FT_PAUSE:

		set_cure_text("Paused");

		break;
	case FT_RESUME:

		

		break;
	case FT_SKIP:


		break;
	case FT_DONE:
		lv_screen_load(ui_screen_finished());
		
		break;
	default:
		break;
	}
}

static void ui_screen_cure_events(lv_event_t *e)
{
	lv_event_code_t event_code = lv_event_get_code(e);

	if (event_code == LV_EVENT_SCREEN_LOADED)
	{
		on_screen_loaded_cb(SCREEN_CURE);

		if (lv_subject_get_int(&subject_time_heat))
		{
			lv_obj_remove_flag(heat_bar_icon, LV_OBJ_FLAG_HIDDEN);
			lv_obj_remove_flag(heat_cont, LV_OBJ_FLAG_HIDDEN);
			obj_remove_state_recursive(heat_cont, LV_STATE_DISABLED, 2);
		}
		else
		{
			lv_obj_add_flag(heat_bar_icon, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(heat_cont, LV_OBJ_FLAG_HIDDEN);
			ui_footer_toggle_buttons(footer, FT_ABORT | FT_PAUSE);
		}

		if (lv_subject_get_int(&subject_switch_cure))
		{
			/* Curing enabled */
			lv_obj_remove_flag(cure_bar_icon, LV_OBJ_FLAG_HIDDEN);
			lv_obj_remove_flag(cure_cont, LV_OBJ_FLAG_HIDDEN);

			if (lv_subject_get_int(&subject_time_heat))
			{
				/* Heating & Cure enabled */
				lv_obj_remove_state(cure_bar_icon, LV_STATE_CHECKED);

				ui_footer_toggle_buttons(footer, FT_ABORT | FT_PAUSE);
				int32_t x = lv_subject_get_int(&subject_time_heat);
				int32_t max = lv_subject_get_int(&subject_time_heat) + lv_subject_get_int(&subject_time_cure);

				int32_t ratio = lv_map(x, 0, max, 180, 1000);

				lv_obj_set_width(progress_bar_heat, ratio);
				lv_obj_set_width(progress_bar_cure, 1180 - ratio);
				lv_obj_remove_flag(progress_bar_heat, LV_OBJ_FLAG_HIDDEN);
				lv_obj_remove_flag(progress_bar_cure, LV_OBJ_FLAG_HIDDEN);

				lv_obj_set_pos(cure_bar_icon, ratio, 0);
			}
			else
			{
				/* Cure only */
				lv_obj_add_state(cure_bar_icon, LV_STATE_CHECKED);
				obj_remove_state_recursive(cure_cont, LV_STATE_DISABLED, 2);
				ui_footer_toggle_buttons(footer, FT_ABORT | FT_PAUSE);

				lv_obj_set_width(progress_bar_heat, 590);
				lv_obj_set_width(progress_bar_cure, 1180);
				lv_obj_add_flag(progress_bar_heat, LV_OBJ_FLAG_HIDDEN);
				lv_obj_remove_flag(progress_bar_cure, LV_OBJ_FLAG_HIDDEN);

				lv_obj_set_pos(cure_bar_icon, 0, 0);
			}
			
			if (lv_subject_get_int(&subject_temperature_cure) <= 34)
			{
				lv_obj_add_flag(title_cont, LV_OBJ_FLAG_HIDDEN);
			}
			else
			{
				lv_obj_remove_flag(title_cont, LV_OBJ_FLAG_HIDDEN);
			}
		}
		else
		{
			/* Cure disabled */
			lv_obj_add_flag(cure_bar_icon, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(cure_cont, LV_OBJ_FLAG_HIDDEN);
			ui_footer_toggle_buttons(footer, FT_ABORT | FT_PAUSE);

			lv_obj_set_width(progress_bar_heat, 1180);
			lv_obj_set_width(progress_bar_cure, 590);
			lv_obj_remove_flag(progress_bar_heat, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(progress_bar_cure, LV_OBJ_FLAG_HIDDEN);
		}

		lv_obj_add_flag(skipped_label, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(paused_cont, LV_OBJ_FLAG_HIDDEN);

		lv_bar_set_value(progress_bar_heat, 0, LV_ANIM_OFF);
		lv_bar_set_value(progress_bar_cure, 0, LV_ANIM_OFF);

	}

	if (event_code == LV_EVENT_SCREEN_UNLOAD_START)
	{
		
	}
}

static void cure_icon_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	LV_UNUSED(observer);
	int32_t v = lv_subject_get_int(subject);

	const lv_image_dsc_t *src = cure_preset_icons[LV_CLAMP(0, v, 5)];

	lv_image_set_src(preset_icon, src);
	if (src == NULL)
	{
		lv_obj_set_width(preset_icon, 0);
	}
	else
	{
		lv_obj_set_width(preset_icon, LV_SIZE_CONTENT);
	}
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
