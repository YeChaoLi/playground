/**
 * @file ui_slider.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui_slider.h"
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

lv_obj_t * ui_slider_create(lv_obj_t * parent)
{
	static bool inited = false;
	static lv_style_t main_style;
	static lv_style_t indic_style;
	static lv_style_t knob_default;

	if(!inited) {
		lv_style_init(&main_style);
		lv_style_set_width(&main_style, 1008);
		lv_style_set_height(&main_style, 14);
		lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
		lv_style_set_bg_color(&main_style, lv_color_hex(0x333333));
		lv_style_set_bg_opa(&main_style, 255);

		lv_style_init(&indic_style);
		lv_style_set_radius(&indic_style, LV_RADIUS_CIRCLE);
		lv_style_set_bg_color(&indic_style, COLOR_INTERACTIVE_ACTIVE);
		lv_style_set_bg_opa(&indic_style, 255);

		lv_style_init(&knob_default);
		lv_style_set_radius(&knob_default, LV_RADIUS_CIRCLE);
		lv_style_set_bg_color(&knob_default, COLOR_INTERACTIVE_ACTIVE);
		lv_style_set_bg_opa(&knob_default, 255);
		lv_style_set_outline_color(&knob_default, COLOR_INTERACTIVE_ACTIVE);
		lv_style_set_outline_opa(&knob_default, 255);
		lv_style_set_outline_width(&knob_default, 13);
		lv_style_set_outline_pad(&knob_default, -1);
		lv_style_set_shadow_color(&knob_default, COLOR_BACKGROUND);
		lv_style_set_shadow_opa(&knob_default, 255);
		lv_style_set_shadow_width(&knob_default, 20);
		lv_style_set_shadow_spread(&knob_default, 9);

		inited = true;
	}


	lv_obj_t * slider_obj = lv_slider_create(parent);
	lv_obj_remove_style_all(slider_obj);
	lv_obj_add_style(slider_obj, &main_style, 0);
	lv_obj_add_style(slider_obj, &indic_style, LV_PART_INDICATOR | LV_STATE_DEFAULT);
	lv_obj_add_style(slider_obj, &knob_default, LV_PART_KNOB| LV_STATE_DEFAULT);

	return slider_obj;
}



/**********************
 *   STATIC FUNCTIONS
 **********************/
