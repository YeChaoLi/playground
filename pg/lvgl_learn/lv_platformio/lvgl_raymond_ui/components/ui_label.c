/**
 * @file ui_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui_label.h"
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

lv_obj_t * ui_label_create(lv_obj_t * parent, const char * text, int32_t width, int32_t height)
{
	static bool inited = false;
	static lv_style_t main_style;
	static lv_style_t disabled_style;
	if(!inited) {
		LV_FONT_DECLARE(font_rubik_28);
		LV_FONT_DECLARE( font_proxima_bold_44);
		lv_style_init(&main_style);
		lv_style_set_bg_opa(&main_style, 255);
		lv_style_set_border_width(&main_style, 3);
		lv_style_set_border_color(&main_style, COLOR_INTERACTIVE_ACTIVE);
		lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
		lv_style_set_text_font(&main_style, &font_proxima_bold_44);
		lv_style_set_text_color(&main_style, COLOR_TEXT_PRIMARY);
		lv_style_set_width(&main_style, width);
		lv_style_set_height(&main_style, 84);

		lv_style_init(&disabled_style);

		inited = true;
	}


	lv_obj_t *cont = lv_obj_create(parent);
	lv_obj_remove_style_all(cont);
	lv_obj_set_width( cont, width);
	lv_obj_set_height( cont, height);

	lv_obj_t *label = lv_label_create(cont);
	lv_obj_set_height( label, LV_SIZE_CONTENT);
	lv_obj_set_width( label, width);
	lv_obj_set_x( label, 0 );
	lv_obj_set_y( label, 0 );
	lv_obj_set_align( label, LV_ALIGN_LEFT_MID );
	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
	lv_label_set_text(label, text);
	lv_obj_set_style_text_color(label, COLOR_TEXT_PRIMARY, 0);
	lv_obj_set_style_text_color(label, COLOR_INTERACTIVE_DISABLED, LV_PART_MAIN | LV_STATE_DISABLED );

	return label;
}



/**********************
 *   STATIC FUNCTIONS
 **********************/
