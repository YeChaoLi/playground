/**
 * @file ui_screen_pre_heat.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../ui.h"
#include "ui_screens.h"
#include "../components/ui_title_bar.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ui_screen_pre_heat_loaded(lv_event_t * e);

static void ui_title_bar_event(lv_event_t * e);
static lv_obj_t * ui_create_label(lv_obj_t * parent, const char * text, int32_t height);
static lv_obj_t * ui_create_diagram(lv_obj_t * parent, const void * mask, const void * line, const char * text, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * pre_heat_screen;
static lv_obj_t * title_bar;
static lv_obj_t * info_cont;
static lv_obj_t * info_label;
static lv_obj_t * diagram_cont;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_screen_pre_heat_init(void)
{
	pre_heat_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(pre_heat_screen, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(pre_heat_screen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(pre_heat_screen, LV_OBJ_FLAG_SCROLLABLE);

	title_bar = ui_title_bar_create(pre_heat_screen, "Pre-heat");

	lv_obj_add_event_cb(title_bar, ui_title_bar_event, LV_EVENT_CLICKED, NULL);

	info_cont = lv_obj_create(pre_heat_screen);
	lv_obj_remove_style_all(info_cont);
	lv_obj_set_width(info_cont, 658);
	lv_obj_set_height(info_cont, 199);
	lv_obj_set_x(info_cont, 20);
	lv_obj_set_y(info_cont, 100);
	lv_obj_set_flex_flow(info_cont,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(info_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_scrollbar_mode(info_cont, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scroll_dir(info_cont, LV_DIR_VER);
	lv_obj_set_style_radius(info_cont, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(info_cont, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(info_cont, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

	LV_FONT_DECLARE(font_proxima_35);
	info_label = ui_create_label(info_cont, "Heat parts to a uniform temperature before curing begins.", LV_SIZE_CONTENT);
	lv_obj_set_style_text_font(info_label, &font_proxima_35, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(info_label, COLOR_TEXT_PRIMARY, 0);


	diagram_cont = lv_obj_create(pre_heat_screen);
	lv_obj_remove_style_all(diagram_cont);
	lv_obj_set_width(diagram_cont, 462);
	lv_obj_set_height(diagram_cont, 180);
	lv_obj_set_x(diagram_cont, 9 + 750);
	lv_obj_set_y(diagram_cont, 50 + 80);
	lv_obj_set_flex_flow(diagram_cont,LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(diagram_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_remove_flag(diagram_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);


	LV_IMG_DECLARE(image_mask_warm);   // assets/mask_warm.png
	LV_IMG_DECLARE(image_line_warm);   // assets/line_warm.png
	LV_IMG_DECLARE(image_mask_heat);   // assets/mask_heat.png
	LV_IMG_DECLARE(image_line_heat);   // assets/line_heat.png
	LV_IMG_DECLARE(image_mask_cure);   // assets/mask_cure.png
	LV_IMG_DECLARE(image_line_cure);   // assets/line_cure.png
	LV_IMG_DECLARE(image_mask_cool);   // assets/mask_cool.png
	LV_IMG_DECLARE(image_line_cool);   // assets/line_cool.png

	ui_create_diagram(diagram_cont, &image_mask_warm, &image_line_warm, "Warmup", COLOR_INTERACTIVE_DISABLED);
	ui_create_diagram(diagram_cont, &image_mask_heat, &image_line_heat, "Pre-heat", COLOR_INTERACTIVE_ACTIVE);
	ui_create_diagram(diagram_cont, &image_mask_cure, &image_line_cure, "Cure", COLOR_INTERACTIVE_DISABLED);
	ui_create_diagram(diagram_cont, &image_mask_cool, &image_line_cool, "Cooling", COLOR_INTERACTIVE_DISABLED);

	lv_obj_add_event_cb(pre_heat_screen, ui_screen_pre_heat_loaded, LV_EVENT_SCREEN_LOADED, NULL);

}

lv_obj_t * ui_screen_pre_heat(void)
{
	if (!pre_heat_screen){
		ui_screen_pre_heat_init();
	}
	return pre_heat_screen;
}

void show_preheat_screen(void){
	lv_screen_load(ui_screen_pre_heat());
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ui_title_bar_event(lv_event_t * e){
	lv_screen_load(ui_screen_home());
}


static lv_obj_t * ui_create_label(lv_obj_t * parent, const char * text, int32_t height)
{
	lv_obj_t *panel = lv_obj_create(parent);
	lv_obj_remove_style_all(panel);
	lv_obj_set_width(panel, lv_pct(100));
	lv_obj_set_height(panel, height);

	lv_obj_t *label = lv_label_create(panel);
	lv_obj_set_height(label, LV_SIZE_CONTENT);
	lv_obj_set_width(label, lv_pct(100));
	lv_obj_set_x(label, 0);
	lv_obj_set_y(label, 0);
	lv_obj_set_align(label, LV_ALIGN_LEFT_MID);
	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
	lv_label_set_text(label, text);

	return label;
}


static lv_obj_t * ui_create_diagram(lv_obj_t * parent, const void * mask, const void * line, const char * text, lv_color_t color)
{
	lv_obj_t *cont = lv_obj_create(parent);
	lv_obj_remove_style_all(cont);
	lv_obj_set_width(cont, 114);
	lv_obj_set_height(cont, 152);
	lv_obj_set_align(cont, LV_ALIGN_CENTER);
	lv_obj_remove_flag(cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t *img_mask = lv_image_create(cont);
	lv_image_set_src(img_mask, mask);
	lv_obj_set_width(img_mask, LV_SIZE_CONTENT);
	lv_obj_set_height(img_mask, LV_SIZE_CONTENT); 
	lv_obj_set_align(img_mask, LV_ALIGN_TOP_MID);
	lv_obj_add_flag(img_mask, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_remove_flag(img_mask, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t *img_line = lv_image_create(cont);
	lv_image_set_src(img_line, line);
	lv_obj_set_width(img_line, LV_SIZE_CONTENT);
	lv_obj_set_height(img_line, LV_SIZE_CONTENT); 
	lv_obj_set_align(img_line, LV_ALIGN_TOP_MID);
	lv_obj_add_flag(img_line, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_remove_flag(img_line, LV_OBJ_FLAG_SCROLLABLE);

	LV_FONT_DECLARE(font_proxima_24);

	lv_obj_t *label = lv_label_create(cont);
	lv_obj_set_width(label, LV_SIZE_CONTENT);
	lv_obj_set_height(label, LV_SIZE_CONTENT); 
	lv_obj_set_x(label, 0);
	lv_obj_set_y(label, 112);
	lv_obj_set_align(label, LV_ALIGN_TOP_MID);
	lv_label_set_text(label, text);
	lv_obj_set_style_text_font(label, &font_proxima_24, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(label, color, 0);

	return cont;
}

static void ui_screen_pre_heat_loaded(lv_event_t * e) {
    on_screen_loaded_cb(SCREEN_PRE_HEAT);
}
