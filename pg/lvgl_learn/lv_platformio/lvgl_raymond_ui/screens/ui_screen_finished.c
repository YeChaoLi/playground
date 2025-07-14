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

static void ui_screen_finished_events(lv_event_t *e);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_obj_t * finished_screen;

static ui_footer_t * footer;

static lv_obj_t *main_cont;
static lv_obj_t *caution_info;
static lv_obj_t *main_label;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_screen_finished_init(void)
{

	finished_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(finished_screen, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(finished_screen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(finished_screen, LV_OBJ_FLAG_SCROLLABLE );

	main_cont = lv_obj_create(finished_screen);
	lv_obj_remove_style_all(main_cont);
	lv_obj_set_width(main_cont, LV_SIZE_CONTENT);
	lv_obj_set_height(main_cont, LV_SIZE_CONTENT);
	lv_obj_set_x(main_cont, 0 );
	lv_obj_set_y(main_cont, 103);
	lv_obj_set_align(main_cont, LV_ALIGN_TOP_MID);
	lv_obj_set_flex_flow(main_cont,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	LV_FONT_DECLARE(font_proxima_bold_128);
	LV_FONT_DECLARE(font_proxima_bold_35);
	main_label = ui_label_create(main_cont, "Finished/Aborted", LV_SIZE_CONTENT, 153);
	lv_obj_set_style_text_font(main_label, &font_proxima_bold_128, 0);
	lv_obj_set_style_text_color(main_label, COLOR_TEXT_PRIMARY, 0);
	lv_obj_set_style_text_align(main_label, LV_TEXT_ALIGN_CENTER, 0);

	caution_info = ui_label_create(main_cont, "Caution: Hot", LV_SIZE_CONTENT, 40);
	lv_obj_set_style_text_font(caution_info, &font_proxima_bold_35, 0);
	lv_obj_set_style_text_color(caution_info, COLOR_TEXT_PRIMARY, 0);
	lv_obj_set_style_text_align(caution_info, LV_TEXT_ALIGN_CENTER, 0);

	/* Footer object */
	footer = ui_footer_create(finished_screen, FT_DONE, ui_footer_events);

	lv_obj_add_event_cb(finished_screen, ui_screen_finished_events, LV_EVENT_ALL, NULL);

}


lv_obj_t * ui_screen_finished(void)
{
	if (!finished_screen){
		ui_screen_finished_init();
	}
	return finished_screen;
}

ui_footer_t *ui_footer_finished(void)
{
	if (!finished_screen){
		ui_screen_finished_init();
	}
	return footer;
}

void show_finished_screen(bool aborted)
{
	if (aborted){
		lv_label_set_text(main_label, "Aborted");
	} else {
		lv_label_set_text(main_label, "Finished");
	}
	lv_screen_load(finished_screen);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ui_footer_events(lv_event_t *e)
{
	footer_button_t button = (footer_button_t)(intptr_t)lv_event_get_user_data(e);

	/* Handle button pressed actions from the callback */
	on_footer_button_pressed_cb(SCREEN_FINISHED, button);

	/* Handle button pressed actions in this file */
	switch (button){
		case FT_DONE:
		lv_screen_load(ui_screen_home());
		break;
		default:
			break;
	}
}

static void ui_screen_finished_events(lv_event_t *e)
{
	lv_event_code_t event_code = lv_event_get_code(e);

	if (event_code == LV_EVENT_SCREEN_LOADED){
		on_screen_loaded_cb(SCREEN_FINISHED);

		if (lv_subject_get_int(&subject_current_temperature) > 50){
			lv_obj_set_y(main_cont, 103);
			lv_obj_remove_flag(caution_info, LV_OBJ_FLAG_HIDDEN);
		} else {
			lv_obj_set_y(main_cont, 123);
			lv_obj_add_flag(caution_info, LV_OBJ_FLAG_HIDDEN);
		}
	}

	if (event_code == LV_EVENT_SCREEN_UNLOADED){

	}
}

