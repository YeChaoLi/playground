/**
 * @file ui_screen_error.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../ui.h"
#include "ui_screens.h"
#include "../components/ui_button.h"
#include "../components/ui_footer.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void ui_screen_error_loaded(lv_event_t * e);
static void ui_footer_events(lv_event_t *e);
static lv_obj_t * ui_create_label(lv_obj_t * parent, const char * text, int32_t height);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * error_screen;
static lv_obj_t * qr_code;
static lv_obj_t * error_cont;
static lv_obj_t * error_title;
static lv_obj_t * error_desc;
static lv_obj_t * error_code;

static ui_footer_t * footer;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_screen_error_init(void)
{
	error_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(error_screen, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(error_screen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(error_screen, LV_OBJ_FLAG_SCROLLABLE);

	/* Footer object */
	footer = ui_footer_create(error_screen, FT_CLOSE, ui_footer_events);

	qr_code = lv_qrcode_create(error_screen);
	lv_obj_remove_style_all(qr_code);
    lv_qrcode_set_size(qr_code, 248);
    lv_qrcode_set_dark_color(qr_code, COLOR_BACKGROUND);
    lv_qrcode_set_light_color(qr_code, COLOR_TEXT_PRIMARY);
	const char * link = "https://support.formlabs.com/";
    lv_qrcode_update(qr_code, link, strlen(link));
	lv_obj_set_x(qr_code, -76);
	lv_obj_set_align(qr_code, LV_ALIGN_RIGHT_MID);
    lv_obj_set_style_border_color(qr_code, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_border_width(qr_code, 2, 0);
    lv_obj_remove_flag(qr_code, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(qr_code, COLOR_TEXT_PRIMARY, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(qr_code, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	error_cont = lv_obj_create(error_screen);
	lv_obj_remove_style_all(error_cont);
	lv_obj_set_width(error_cont, 950);
	lv_obj_set_height(error_cont, 201);
	lv_obj_set_x(error_cont, 60);
	lv_obj_set_y(error_cont, 60);
	lv_obj_set_flex_flow(error_cont,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(error_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_scrollbar_mode(error_cont, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scroll_dir(error_cont, LV_DIR_VER);
	lv_obj_set_style_radius(error_cont, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(error_cont, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(error_cont, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_row(error_cont, 20, LV_PART_MAIN| LV_STATE_DEFAULT);

	LV_FONT_DECLARE(font_proxima_bold_44);
	error_title = ui_create_label(error_cont, "Error title", 60);
	lv_obj_set_style_text_font(error_title, &font_proxima_bold_44, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(error_title, COLOR_STATUS_NEGATIVE, 0);

	LV_FONT_DECLARE(font_proxima_35);
	error_desc = ui_create_label(error_cont, "Error description", 40);
	lv_obj_set_style_text_font(error_desc, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(error_desc, COLOR_TEXT_PRIMARY, 0);

	LV_FONT_DECLARE(font_proxima_28);
	error_code = ui_create_label(error_cont, "Error code: 404", 40);
	lv_obj_set_style_text_font(error_code, &font_proxima_28, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(error_code, COLOR_TEXT_SECONDARY, 0);

	lv_obj_add_event_cb(error_screen, ui_screen_error_loaded, LV_EVENT_SCREEN_LOADED, NULL);

}

lv_obj_t * ui_screen_error(void)
{
	if (!error_screen){
		ui_screen_error_init();
	}
	return error_screen;
}

ui_footer_t *ui_footer_error(void)
{
	if (!error_screen){
		ui_screen_error_init();
	}
	return footer;
}


void show_error(const char * title, const char * desc, int32_t code, const char * link)
{
	lv_qrcode_update(qr_code, link, strlen(link));
	lv_label_set_text(error_title, title);
	lv_label_set_text(error_desc, desc);
	lv_label_set_text_fmt(error_code, "Error code: %d", code);

	lv_screen_load(ui_screen_error());
}


/**********************
 *   STATIC FUNCTIONS
 **********************/


static void ui_screen_error_loaded(lv_event_t * e)
{
	on_screen_loaded_cb(SCREEN_ERROR);
}

static void ui_footer_events(lv_event_t *e)
{
	footer_button_t button = (footer_button_t)(intptr_t)lv_event_get_user_data(e);

	/* Handle button pressed actions from the callback */
	on_footer_button_pressed_cb(SCREEN_ERROR, button);

	/* Handle button pressed actions in this file */
	switch (button){
		case FT_CLOSE:
		lv_screen_load(ui_screen_home());
		break;
		default:
			break;
	}

}

static lv_obj_t * ui_create_label(lv_obj_t * parent, const char * text, int32_t height)
{
	lv_obj_t *panel = lv_obj_create(parent);
	lv_obj_remove_style_all(panel);
	lv_obj_set_width(panel, lv_pct(100));
	lv_obj_set_height(panel, height);

	lv_obj_t *label = lv_label_create(panel);
	lv_obj_set_height(label, LV_SIZE_CONTENT);
	lv_obj_set_width(label, LV_SIZE_CONTENT);
	lv_obj_set_x(label, 0);
	lv_obj_set_y(label, 0);
	lv_obj_set_align(label, LV_ALIGN_LEFT_MID);
	lv_label_set_text(label, text);

	return label;
}
