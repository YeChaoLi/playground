/**
 * @file ui_screen_door.c
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

static void ui_screen_door_loaded(lv_event_t * e);
static void ui_footer_events(lv_event_t *e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * door_screen;
static lv_obj_t * logo_image;

static ui_footer_t * footer;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_screen_door_init(void)
{
	door_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(door_screen, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(door_screen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(door_screen, LV_OBJ_FLAG_SCROLLABLE);


	lv_obj_t * main_cont = lv_obj_create(door_screen);
	lv_obj_remove_style_all(main_cont);
	lv_obj_set_width(main_cont, 950);
	lv_obj_set_height(main_cont, 500);
	lv_obj_set_x(main_cont, 60);
	lv_obj_set_y(main_cont, 60);
	lv_obj_set_align(main_cont, LV_ALIGN_TOP_LEFT);
	lv_obj_set_flex_flow(main_cont,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(main_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_color(main_cont, COLOR_INTERACTIVE_SECONDARY, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(main_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_row(main_cont, 20, LV_PART_MAIN| LV_STATE_DEFAULT);

	LV_FONT_DECLARE(font_proxima_35);
	LV_FONT_DECLARE(font_proxima_bold_44);

	lv_obj_t * label;
	label = ui_label_create(main_cont, "Door open", LV_SIZE_CONTENT, 60);
	lv_obj_set_style_text_font(label, &font_proxima_bold_44, 0);
	lv_obj_set_style_text_color(label, COLOR_STATUS_NEGATIVE, 0);

	label = ui_label_create(main_cont, "Close door to resume", LV_SIZE_CONTENT, 40);
	lv_obj_set_style_text_font(label, &font_proxima_35, 0);
	lv_obj_set_style_text_color(label, COLOR_TEXT_PRIMARY, 0);


	LV_IMAGE_DECLARE(image_open_door);
	lv_obj_t * image = lv_image_create(door_screen);
	lv_image_set_src(image, &image_open_door);
	lv_obj_set_width(image, LV_SIZE_CONTENT);
	lv_obj_set_height(image, LV_SIZE_CONTENT); 
	lv_obj_set_x(image, 930);
	lv_obj_set_y(image, 50);
	lv_obj_set_align(image, LV_ALIGN_TOP_LEFT);
	lv_obj_add_flag(image, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_remove_flag(image, LV_OBJ_FLAG_SCROLLABLE);

	/* Footer object */
	footer = ui_footer_create(door_screen, FT_ABORT, ui_footer_events);

	lv_obj_add_event_cb(door_screen, ui_screen_door_loaded, LV_EVENT_SCREEN_LOADED, NULL);

}

lv_obj_t * ui_screen_door(void)
{
	if (!door_screen){
		ui_screen_door_init();
	}
	return door_screen;
}

ui_footer_t *ui_footer_door(void)
{
	if (!door_screen){
		ui_screen_door_init();
	}
	return footer;
}

void door_open(){
	lv_screen_load(ui_screen_door());
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ui_screen_door_loaded(lv_event_t * e)
{
	on_screen_loaded_cb(SCREEN_DOOR);
}


static void ui_footer_events(lv_event_t *e)
{
	footer_button_t button = (footer_button_t)(intptr_t)lv_event_get_user_data(e);

	/* Handle button pressed actions from the callback */
	on_footer_button_pressed_cb(SCREEN_DOOR, button);

	/* Handle button pressed actions in this file */
	switch (button){
		case FT_ABORT:
			// lv_screen_load(ui_screen_home());
			show_finished_screen(true);
			break;
		default:
			break;
	}

}
