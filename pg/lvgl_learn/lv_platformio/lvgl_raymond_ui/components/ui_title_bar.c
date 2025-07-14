/**
 * @file ui_title_bar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui_title_bar.h"
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

lv_obj_t * ui_title_bar_create(lv_obj_t * parent, const char * title)
{
	lv_obj_t *cont = lv_obj_create(parent);
	lv_obj_remove_style_all(cont);
	lv_obj_set_height(cont, 100);
	lv_obj_set_width(cont, lv_pct(100));
	lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLLABLE );
	lv_obj_set_style_bg_color(cont, COLOR_INTERACTIVE_SECONDARY, LV_PART_MAIN | LV_STATE_PRESSED );
	lv_obj_set_style_bg_opa(cont, 255, LV_PART_MAIN| LV_STATE_PRESSED);
	lv_obj_remove_flag(cont, LV_OBJ_FLAG_PRESS_LOCK);

	LV_IMAGE_DECLARE(image_button_back);

	lv_obj_t *icon = lv_image_create(cont);
	lv_image_set_src(icon, &image_button_back);
	lv_obj_set_width(icon, LV_SIZE_CONTENT);
	lv_obj_set_height(icon, LV_SIZE_CONTENT); 
	lv_obj_set_align(icon, LV_ALIGN_LEFT_MID );
	lv_obj_remove_flag(icon, LV_OBJ_FLAG_CLICKABLE );
	lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE );


	LV_FONT_DECLARE(font_proxima_bold_44);
	lv_obj_t *text = lv_label_create(cont);
	lv_obj_set_width(text, LV_SIZE_CONTENT);
	lv_obj_set_height(text, LV_SIZE_CONTENT); 
	lv_obj_set_x(text, 80 );
	lv_obj_set_y(text, 0 );
	lv_obj_set_align(text, LV_ALIGN_LEFT_MID );
	lv_label_set_text(text, title);
	lv_obj_set_style_text_font(text, &font_proxima_bold_44, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(text, COLOR_TEXT_PRIMARY, 0);

	return cont;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
