/**
 * @file ui_list_element.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui_list_element.h"
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

lv_obj_t * ui_list_element_create(lv_obj_t * parent, const void *icon, const char * text)
{
	static bool inited = false;
	static lv_style_t main_style;
	static lv_style_t checked_style;
	if(!inited) {
		LV_FONT_DECLARE(font_proxima_35);
		lv_style_init(&main_style);
		lv_style_set_layout(&main_style, LV_LAYOUT_FLEX);
		lv_style_set_flex_flow(&main_style, LV_FLEX_FLOW_ROW);
		lv_style_set_flex_cross_place(&main_style, LV_FLEX_ALIGN_CENTER);
		lv_style_set_flex_track_place(&main_style, LV_FLEX_ALIGN_CENTER);
		lv_style_set_width(&main_style, lv_pct(100));
		lv_style_set_height(&main_style, 84);
		lv_style_set_pad_hor(&main_style, 18);
		lv_style_set_pad_column(&main_style, 8);
		lv_style_set_pad_left(&main_style, 0);
		lv_style_set_pad_right(&main_style, 0);
		lv_style_set_text_color(&main_style, lv_color_hex(0xffffff));
		lv_style_set_radius(&main_style, 0);
		lv_style_set_border_width(&main_style, 2);
		lv_style_set_text_font(&main_style, &font_proxima_35);
		lv_style_set_border_color(&main_style, COLOR_LINE);
		lv_style_set_border_opa(&main_style, LV_OPA_100);
		lv_style_set_border_side(&main_style, LV_BORDER_SIDE_BOTTOM);
		lv_style_set_bg_color(&main_style,COLOR_BACKGROUND);
		lv_style_set_bg_opa(&main_style, 0);

		lv_style_init(&checked_style);
		lv_style_set_border_side(&checked_style, LV_BORDER_SIDE_FULL);
		lv_style_set_border_color(&main_style, COLOR_INTERACTIVE_DISABLED);
		lv_style_set_bg_color(&checked_style, COLOR_SURFACE);
		lv_style_set_height(&checked_style, 86);
		lv_style_set_pad_hor(&checked_style, 18);
		lv_style_set_radius(&checked_style, 8);
		lv_style_set_bg_opa(&checked_style, LV_OPA_100);
		lv_style_set_pad_left(&checked_style, 10);
		lv_style_set_pad_right(&checked_style, 10);
		inited = true;
	}

	lv_obj_t * cont = lv_button_create(parent);
	lv_obj_remove_style_all(cont);
	lv_obj_add_style(cont, &main_style, 0);
	lv_obj_add_style(cont, &checked_style, LV_STATE_CHECKED);
	lv_obj_remove_flag(cont, LV_OBJ_FLAG_CLICK_FOCUSABLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_EVENT_BUBBLE);
	lv_obj_add_flag(cont, LV_OBJ_FLAG_SNAPPABLE);

	lv_obj_t * img = lv_image_create(cont);
	lv_image_set_src(img, icon);

	lv_obj_t * label = lv_label_create(cont);
	lv_label_set_text(label, text);
	lv_obj_set_flex_grow(label, 1);

	LV_IMAGE_DECLARE(image_forward_arrow);
	lv_obj_t * arrow = lv_image_create(cont);
	lv_image_set_src(arrow, &image_forward_arrow);

	return cont;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
