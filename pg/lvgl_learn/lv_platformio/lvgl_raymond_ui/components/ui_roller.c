/**
 * @file ui_roller.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui_roller.h"
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
static void generate_mask(lv_draw_buf_t * mask, lv_obj_t *parent);

static void on_value_change_cb(lv_event_t *e);
static void value_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

/**********************
 *  STATIC VARIABLES
 **********************/

static const char *temp_options = "100\n95\n90\n85\n80\n75\n70\n65\n60\n55\n50\n45\n40\n35\nOFF";

static const char *time_options = "120\n115\n110\n105\n100\n95\n90\n85\n80\n75\n70\n65\n60\n55\n50\n45\n40\n35\n30\n29\n28\n27\n26\n25\n24\n23\n22\n21\n20\n19\n18\n17\n16\n15\n14\n13\n12\n11\n10\n9\n8\n7\n6\n5\n4\n3\n2\n1\n0";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *ui_roller_create(lv_obj_t *parent, bool temp, lv_subject_t * subject)
{
	static bool inited = false;
	static lv_style_t main_style;
	static lv_style_t selected_style;
	static lv_style_t disabled_style;
	if (!inited)
	{
		LV_FONT_DECLARE(font_proxima_50);
		lv_style_init(&main_style);
		lv_style_set_bg_opa(&main_style, LV_OPA_0);
		lv_style_set_bg_color(&main_style, COLOR_SURFACE);
		lv_style_set_border_width(&main_style, 0);
		lv_style_set_border_opa(&main_style, 0);
		lv_style_set_border_color(&main_style, COLOR_INTERACTIVE_DISABLED);
		lv_style_set_radius(&main_style, 8);
		lv_style_set_text_font(&main_style, &font_proxima_50);
		lv_style_set_text_color(&main_style, COLOR_TEXT_PRIMARY);
		lv_style_set_width(&main_style, 120);
		lv_style_set_height(&main_style, 199);

		lv_style_init(&selected_style);
		lv_style_set_bg_opa(&selected_style, LV_OPA_0);
		lv_style_set_bg_color(&selected_style, lv_color_hex(0x272727));
		lv_style_set_radius(&selected_style, 8);
		lv_style_set_border_width(&selected_style, 0);
		lv_style_set_border_opa(&selected_style, 0);
		lv_style_set_border_color(&selected_style, COLOR_INTERACTIVE_DISABLED);

		lv_style_init(&disabled_style);
		lv_style_set_border_color(&disabled_style, COLOR_LINE);
		lv_style_set_text_color(&disabled_style, COLOR_INTERACTIVE_DISABLED);

		inited = true;
	}

	lv_obj_t *cont = lv_obj_create(parent);
	lv_obj_remove_style_all(cont);
	lv_obj_set_width(cont, 120);
	lv_obj_set_height(cont, 199);

	lv_obj_t *panel = lv_obj_create(cont);
	lv_obj_set_width(panel, 120);
	lv_obj_set_height(panel, 84);
	// lv_obj_set_y(panel, -5);
	lv_obj_set_align(panel, LV_ALIGN_CENTER);
	lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_radius(panel, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(panel, lv_color_hex(0x616161), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(panel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);

	lv_obj_t *roller = lv_roller_create(cont);
	lv_obj_add_style(roller, &main_style, 0);
	lv_obj_add_style(roller, &selected_style, LV_PART_SELECTED| LV_STATE_DEFAULT);
	lv_obj_add_style(roller, &disabled_style, LV_PART_SELECTED| LV_STATE_DISABLED);
	lv_obj_set_align(roller, LV_ALIGN_CENTER);
	lv_obj_set_style_text_line_space(roller, 20, LV_PART_MAIN| LV_STATE_DEFAULT);
	// lv_obj_set_y(roller, 5);
	lv_obj_set_user_data(roller, (void *)temp);

	if (temp){
		lv_roller_set_options(roller, temp_options, LV_ROLLER_MODE_NORMAL);
	} else {
		lv_roller_set_options(roller, time_options, LV_ROLLER_MODE_NORMAL);
	}
	

	lv_obj_add_event_cb(roller, on_value_change_cb, LV_EVENT_VALUE_CHANGED, subject);

	lv_subject_add_observer(subject, value_observer_cb, roller);

	// lv_roller_set_visible_row_count(roller, 4);

	/* Create the mask to make the top and bottom part of roller faded.
	 * The width and height are empirical values for simplicity*/
	LV_DRAW_BUF_DEFINE_STATIC(mask, 120, 199, LV_COLOR_FORMAT_L8);
	LV_DRAW_BUF_INIT_STATIC(mask);

	generate_mask(&mask, cont);
	lv_obj_set_style_bitmap_mask_src(roller, &mask, 0);

	return cont;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void generate_mask(lv_draw_buf_t *mask, lv_obj_t *parent)
{
	/*Create a "8 bit alpha" canvas and clear it*/
	lv_obj_t *canvas = lv_canvas_create(parent);
	lv_canvas_set_draw_buf(canvas, mask);
	lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_TRANSP);

	lv_layer_t layer;
	lv_canvas_init_layer(canvas, &layer);

	/*Draw a label to the canvas. The result "image" will be used as mask*/
	lv_draw_rect_dsc_t rect_dsc;
	lv_draw_rect_dsc_init(&rect_dsc);
	rect_dsc.bg_grad.dir = LV_GRAD_DIR_VER;
	rect_dsc.bg_grad.stops[0].color = lv_color_black();
	rect_dsc.bg_grad.stops[1].color = lv_color_white();
	rect_dsc.bg_grad.stops[0].opa = LV_OPA_COVER;
	rect_dsc.bg_grad.stops[1].opa = LV_OPA_COVER;
	lv_area_t a = {0, 0, mask->header.w - 1, mask->header.h / 2 - 10};
	lv_draw_rect(&layer, &rect_dsc, &a);

	a.y1 = mask->header.h / 2 + 10;
	a.y2 = mask->header.h - 1;
	rect_dsc.bg_grad.stops[0].color = lv_color_white();
	rect_dsc.bg_grad.stops[1].color = lv_color_black();
	lv_draw_rect(&layer, &rect_dsc, &a);

	lv_canvas_finish_layer(canvas, &layer);

	/*Comment it to make the mask visible*/
	lv_obj_delete(canvas);
}

static void on_value_change_cb(lv_event_t *e)
{
	lv_subject_t * subject = lv_event_get_user_data(e);
	lv_obj_t * roller = lv_event_get_target(e);
	int32_t type = (int32_t)lv_obj_get_user_data(roller);
	int32_t value = lv_roller_get_selected(roller);

	if (type) {
		/* Temperature */
		value = 100 - (value * 5);
	} else {
		if (value <= 18){
			value = 120 - (value * 5);
		} else {
			value = 30 - (value - 18);
		}
	}
	lv_subject_set_int(subject, value);
}

static void value_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{

	lv_obj_t *roller = (lv_obj_t *)lv_observer_get_user_data(observer);
	int32_t value = lv_subject_get_int(subject);
	int32_t type = (int32_t)lv_obj_get_user_data(roller);

	if (type) {
		/* Temperature */
		value = (100 - value) / 5;
	} else {
		if (value >= 30){
			value = (120 - value) / 5;
		} else {
			value = 30 - value + 18;
		}
	}
	lv_roller_set_selected(roller, value, LV_ANIM_ON);
}

