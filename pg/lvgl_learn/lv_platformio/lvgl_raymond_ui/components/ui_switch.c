/**
 * @file ui_switch.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui_switch.h"
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

lv_obj_t * ui_switch_create(lv_obj_t * parent)
{
	static bool inited = false;
	static lv_style_t main_style;
	static lv_style_t checked_style;
	static lv_style_t indic_checked;
	static lv_style_t knob_default;
	static lv_style_t knob_checked;

	if(!inited) {
		lv_style_init(&main_style);
		lv_style_set_width(&main_style, 76);
		lv_style_set_height(&main_style, 28);
		lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
		lv_style_set_bg_color(&main_style, COLOR_TEXT_SECONDARY);
		lv_style_set_bg_opa(&main_style, 255);

		lv_style_init(&checked_style);
		lv_style_set_bg_color(&checked_style, COLOR_INTERACTIVE_PRESSED);
		lv_style_set_bg_opa(&checked_style, 255);

		lv_style_init(&indic_checked);
		lv_style_set_radius(&indic_checked, LV_RADIUS_CIRCLE);
		lv_style_set_bg_color(&indic_checked, COLOR_INTERACTIVE_PRESSED);
		lv_style_set_bg_opa(&indic_checked, 255);

		lv_style_init(&knob_default);
		lv_style_set_radius(&knob_default, LV_RADIUS_CIRCLE);
		lv_style_set_bg_color(&knob_default, COLOR_TEXT_PRIMARY);
		lv_style_set_bg_opa(&knob_default, 255);
		lv_style_set_outline_color(&knob_default, COLOR_TEXT_PRIMARY);
		lv_style_set_outline_opa(&knob_default, 255);
		lv_style_set_outline_width(&knob_default, 10);
		lv_style_set_outline_pad(&knob_default, -1);

		lv_style_init(&knob_checked);
		lv_style_set_bg_color(&knob_checked, COLOR_INTERACTIVE_ACTIVE);
		lv_style_set_bg_opa(&knob_checked, 255);
		lv_style_set_outline_color(&knob_checked, COLOR_INTERACTIVE_ACTIVE);
		lv_style_set_outline_opa(&knob_checked, 255);
		lv_style_set_outline_width(&knob_checked, 10);

		inited = true;
	}


	lv_obj_t * switch_obj = lv_switch_create(parent);
	lv_obj_remove_style_all(switch_obj);
	lv_obj_add_style(switch_obj, &main_style, 0);
	lv_obj_add_style(switch_obj, &checked_style, LV_PART_MAIN | LV_STATE_CHECKED );
	lv_obj_add_style(switch_obj, &indic_checked, LV_PART_INDICATOR | LV_STATE_CHECKED);
	lv_obj_add_style(switch_obj, &knob_default, LV_PART_KNOB| LV_STATE_DEFAULT);
	lv_obj_add_style(switch_obj, &knob_checked, LV_PART_KNOB| LV_STATE_CHECKED);
	lv_obj_set_ext_click_area(switch_obj, 40);

	return switch_obj;
}



/**********************
 *   STATIC FUNCTIONS
 **********************/
