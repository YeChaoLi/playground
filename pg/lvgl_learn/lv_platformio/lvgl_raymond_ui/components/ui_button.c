/**
 * @file ui_button.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui_button.h"
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

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * ui_button_primary_create(lv_obj_t * parent, const char * text)
{
	static bool inited = false;
	static lv_style_t main_style;
	static lv_style_t disabled_style;
	static lv_style_t pressed_style;
	if(!inited) {
		LV_FONT_DECLARE(font_proxima_bold_35);
		lv_style_init(&main_style);
		lv_style_set_bg_color(&main_style, COLOR_INTERACTIVE_ACTIVE);
		lv_style_set_bg_opa(&main_style, 255);
		lv_style_set_border_width(&main_style, 3);
		lv_style_set_border_color(&main_style, COLOR_INTERACTIVE_ACTIVE);
		lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
		lv_style_set_text_font(&main_style, &font_proxima_bold_35);
		lv_style_set_text_color(&main_style, COLOR_TEXT_PRIMARY);
		lv_style_set_width(&main_style, 240);
		lv_style_set_height(&main_style, 84);

		lv_style_init(&disabled_style);
		lv_style_set_bg_opa(&disabled_style, 0);
		lv_style_set_border_width(&disabled_style, 3);
		lv_style_set_border_color(&disabled_style, COLOR_INTERACTIVE_DISABLED);
		lv_style_set_text_color(&disabled_style, COLOR_INTERACTIVE_DISABLED);

		lv_style_init(&pressed_style);
		lv_style_set_bg_color(&pressed_style, COLOR_INTERACTIVE_PRESSED);
		lv_style_set_bg_opa(&pressed_style, 255);
		lv_style_set_border_width(&pressed_style, 3);
		lv_style_set_border_color(&pressed_style, COLOR_INTERACTIVE_PRESSED);
		lv_style_set_text_color(&pressed_style, COLOR_INTERACTIVE_SECONDARY);

		inited = true;
	}

	lv_obj_t * btn = lv_button_create(parent);
	lv_obj_remove_style_all(btn);
	lv_obj_add_style(btn, &main_style, 0);
	lv_obj_add_style(btn, &disabled_style, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_add_style(btn, &pressed_style, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_remove_flag(btn, LV_OBJ_FLAG_PRESS_LOCK);

	lv_obj_t * label = lv_label_create(btn);
	lv_label_set_text(label, text);
	lv_obj_center(label);

	return btn;
}

lv_obj_t * ui_button_secondary_create(lv_obj_t * parent, const char * text)
{
	static bool inited = false;
	static lv_style_t main_style;
	static lv_style_t disabled_style;
	static lv_style_t pressed_style;
	if(!inited) {
		LV_FONT_DECLARE(font_proxima_bold_35);
		lv_style_init(&main_style);
		lv_style_set_bg_opa(&main_style, 255);
		lv_style_set_bg_color(&main_style, COLOR_BACKGROUND);
		lv_style_set_border_width(&main_style, 3);
		lv_style_set_border_color(&main_style, COLOR_INTERACTIVE_ACTIVE);
		lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
		lv_style_set_text_font(&main_style, &font_proxima_bold_35);
		lv_style_set_text_color(&main_style, COLOR_TEXT_PRIMARY);
		lv_style_set_width(&main_style, 240);
		lv_style_set_height(&main_style, 84);

		lv_style_init(&disabled_style);
		lv_style_set_bg_opa(&disabled_style, 0);
		lv_style_set_border_width(&disabled_style, 3);
		lv_style_set_border_color(&disabled_style, COLOR_INTERACTIVE_DISABLED);
		lv_style_set_text_color(&disabled_style, COLOR_INTERACTIVE_DISABLED);

		lv_style_init(&pressed_style);
		lv_style_set_bg_opa(&pressed_style, 0);
		lv_style_set_border_width(&pressed_style, 3);
		lv_style_set_border_color(&pressed_style, COLOR_INTERACTIVE_PRESSED);
		lv_style_set_text_color(&pressed_style, COLOR_INTERACTIVE_SECONDARY);

		inited = true;
	}

	lv_obj_t * btn = lv_button_create(parent);
	lv_obj_remove_style_all(btn);
	lv_obj_add_style(btn, &main_style, 0);
	lv_obj_add_style(btn, &disabled_style, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_add_style(btn, &pressed_style, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_remove_flag(btn, LV_OBJ_FLAG_PRESS_LOCK);

	lv_obj_t * label = lv_label_create(btn);
	lv_label_set_text(label, text);
	lv_obj_center(label);

	return btn;
}

lv_obj_t * ui_button_destructive_create(lv_obj_t * parent, const char * text)
{
	static bool inited = false;
	static lv_style_t main_style;
	static lv_style_t disabled_style;
	static lv_style_t pressed_style;
	if(!inited) {
		LV_FONT_DECLARE(font_proxima_bold_35);
		lv_style_init(&main_style);
		lv_style_set_bg_color(&main_style, COLOR_BACKGROUND);
		lv_style_set_bg_opa(&main_style, 255);
		lv_style_set_border_width(&main_style, 3);
		lv_style_set_border_color(&main_style, COLOR_STATUS_NEGATIVE);
		lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
		lv_style_set_text_font(&main_style, &font_proxima_bold_35);
		lv_style_set_text_color(&main_style, COLOR_TEXT_PRIMARY);
		lv_style_set_width(&main_style, 240);
		lv_style_set_height(&main_style, 84);

		lv_style_init(&disabled_style);
		lv_style_set_bg_opa(&disabled_style, 0);
		lv_style_set_border_width(&disabled_style, 3);
		lv_style_set_border_color(&disabled_style, COLOR_INTERACTIVE_DISABLED);
		lv_style_set_text_color(&disabled_style, COLOR_INTERACTIVE_DISABLED);

		lv_style_init(&pressed_style);
		lv_style_set_bg_opa(&pressed_style, 0);
		lv_style_set_border_width(&pressed_style, 3);
		lv_style_set_border_color(&pressed_style, COLOR_STATUS_NEGATIVE_PRESSED);
		lv_style_set_text_color(&pressed_style, COLOR_INTERACTIVE_SECONDARY);

		inited = true;
	}

	lv_obj_t * btn = lv_button_create(parent);
	lv_obj_remove_style_all(btn);
	lv_obj_add_style(btn, &main_style, 0);
	lv_obj_add_style(btn, &disabled_style, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_add_style(btn, &pressed_style, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_remove_flag(btn, LV_OBJ_FLAG_PRESS_LOCK);

	lv_obj_t * label = lv_label_create(btn);
	lv_label_set_text(label, text);
	lv_obj_center(label);

	return btn;
}


lv_obj_t * ui_button_icon_create(lv_obj_t * parent, const void * src)
{
	static bool inited = false;
	static lv_style_t main_style;
	static lv_style_t disabled_style;
	static lv_style_t pressed_style;
	if(!inited) {
		LV_FONT_DECLARE(font_rubik_28);
		lv_style_init(&main_style);
		lv_style_set_bg_color(&main_style, COLOR_BACKGROUND);
		lv_style_set_bg_opa(&main_style, 255);
		lv_style_set_border_width(&main_style, 3);
		lv_style_set_border_color(&main_style, COLOR_TEXT_PRIMARY);
		lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
		lv_style_set_text_font(&main_style, &font_rubik_28);
		lv_style_set_text_color(&main_style, COLOR_TEXT_PRIMARY);
		lv_style_set_width(&main_style, 84);
		lv_style_set_height(&main_style, 84);

		lv_style_init(&disabled_style);
		lv_style_set_bg_opa(&disabled_style, 0);
		lv_style_set_border_width(&disabled_style, 3);
		lv_style_set_border_color(&disabled_style, COLOR_INTERACTIVE_DISABLED);
		lv_style_set_text_color(&disabled_style, COLOR_INTERACTIVE_DISABLED);
		lv_style_set_image_recolor(&disabled_style, COLOR_INTERACTIVE_DISABLED);
		lv_style_set_image_recolor_opa(&disabled_style, 255);

		lv_style_init(&pressed_style);
		lv_style_set_bg_opa(&pressed_style, 0);
		lv_style_set_border_width(&pressed_style, 3);
		lv_style_set_border_color(&pressed_style, COLOR_INTERACTIVE_SECONDARY);
		lv_style_set_text_color(&pressed_style, COLOR_INTERACTIVE_SECONDARY);
		lv_style_set_image_recolor(&pressed_style, COLOR_INTERACTIVE_SECONDARY);
		lv_style_set_image_recolor_opa(&pressed_style, 255);

		inited = true;
	}

	lv_obj_t * btn = lv_button_create(parent);
	lv_obj_remove_style_all(btn);
	lv_obj_add_style(btn, &main_style, 0);
	lv_obj_add_style(btn, &disabled_style, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_add_style(btn, &pressed_style, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_remove_flag(btn, LV_OBJ_FLAG_PRESS_LOCK);

	lv_obj_t * icon = lv_image_create(btn);
	lv_image_set_src(icon, src);
	lv_obj_set_width(icon, lv_pct(100));
	lv_obj_set_height(icon, lv_pct(100));
	lv_obj_set_align(icon, LV_ALIGN_CENTER);
	lv_obj_add_flag(icon, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_flag(icon, LV_OBJ_FLAG_EVENT_BUBBLE);
	lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_remove_flag(icon, LV_OBJ_FLAG_PRESS_LOCK);
	lv_obj_set_style_image_recolor(icon, COLOR_INTERACTIVE_DISABLED, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_set_style_image_recolor_opa(icon, 255, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_set_style_image_recolor(icon, COLOR_INTERACTIVE_SECONDARY, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_style_image_recolor_opa(icon, 255, LV_PART_MAIN | LV_STATE_PRESSED);


	return btn;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
