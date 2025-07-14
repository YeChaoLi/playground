/**
 * @file ui_spinbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui_spinbox.h"
#include "../ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void increment_cb(lv_event_t * e);
static void decrement_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * ui_spinbox_create(lv_obj_t * parent, lv_subject_t * subject)
{
	static bool inited = false;
	static lv_style_t main_style;
	static lv_style_t disabled_style;
	if(!inited) {
		LV_FONT_DECLARE(font_proxima_50);
		lv_style_init(&main_style);
		lv_style_set_bg_opa(&main_style, LV_OPA_0);
		lv_style_set_bg_color(&main_style, COLOR_SURFACE);
		lv_style_set_border_width(&main_style, 2);
		lv_style_set_border_opa(&main_style, 255);
		lv_style_set_border_color(&main_style, COLOR_INTERACTIVE_DISABLED);
		lv_style_set_radius(&main_style, 8);
		lv_style_set_text_font(&main_style, &font_proxima_50);
		lv_style_set_text_color(&main_style, COLOR_TEXT_PRIMARY);
		lv_style_set_width(&main_style, lv_pct(100));
		lv_style_set_height(&main_style, 84);

		lv_style_init(&disabled_style);
		lv_style_set_border_color(&disabled_style, COLOR_LINE);
		lv_style_set_text_color(&disabled_style, COLOR_INTERACTIVE_DISABLED);

		inited = true;
	}

	lv_obj_t * cont = lv_obj_create(parent);
	lv_obj_remove_style_all(cont);
	lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_width(cont, 120);
	lv_obj_set_height(cont, 180);
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	LV_IMAGE_DECLARE(image_caret_up);
	lv_obj_t * caret_up = lv_image_create(cont);
	lv_obj_set_width(caret_up, lv_pct(100));
	lv_image_set_src(caret_up, &image_caret_up);
	lv_obj_add_flag(caret_up, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_event_cb(caret_up, increment_cb, LV_EVENT_CLICKED, subject);
	lv_obj_add_event_cb(caret_up, increment_cb, LV_EVENT_LONG_PRESSED_REPEAT, subject);
	lv_obj_set_style_image_recolor(caret_up, COLOR_INTERACTIVE_DISABLED, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_style_image_recolor_opa(caret_up, 255, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_style_image_recolor(caret_up, COLOR_INTERACTIVE_DISABLED, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_set_style_image_recolor_opa(caret_up, 255, LV_PART_MAIN | LV_STATE_DISABLED);

	lv_obj_t * box = lv_obj_create(cont);
	lv_obj_remove_style_all(box);
	lv_obj_add_style(box, &main_style, 0);
	lv_obj_add_style(box, &disabled_style, LV_PART_MAIN | LV_STATE_DISABLED);

	lv_obj_t * label =  lv_label_create(box);
	lv_obj_center(label);
	lv_label_bind_text(label, subject, "%d");
	lv_obj_set_y( label, 3);

	LV_IMAGE_DECLARE(image_caret_down);
	lv_obj_t * caret_down = lv_image_create(cont);
	lv_obj_set_width(caret_down, lv_pct(100));
	lv_image_set_src(caret_down, &image_caret_down);
	lv_obj_add_flag(caret_down, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_event_cb(caret_down, decrement_cb, LV_EVENT_CLICKED, subject);
	lv_obj_add_event_cb(caret_down, decrement_cb, LV_EVENT_LONG_PRESSED_REPEAT, subject);
	lv_obj_set_style_image_recolor(caret_down, COLOR_INTERACTIVE_DISABLED, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_style_image_recolor_opa(caret_down, 255, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_style_image_recolor(caret_down, COLOR_INTERACTIVE_DISABLED, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_set_style_image_recolor_opa(caret_down, 255, LV_PART_MAIN | LV_STATE_DISABLED);
	return cont;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void increment_cb(lv_event_t * e)
{
	lv_subject_t * subject = lv_event_get_user_data(e);
	lv_subject_set_int(subject, lv_subject_get_int(subject) + 1);
}

static void decrement_cb(lv_event_t * e)
{
	lv_subject_t * subject = lv_event_get_user_data(e);
	lv_subject_set_int(subject, lv_subject_get_int(subject) - 1);
}
