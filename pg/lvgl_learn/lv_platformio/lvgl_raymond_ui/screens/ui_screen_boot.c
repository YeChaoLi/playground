/**
 * @file ui_screen_boot.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../ui.h"
#include "ui_screens.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ui_screen_boot_loaded(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * boot_screen;
static lv_obj_t * logo_image;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_screen_boot_init(void)
{
	boot_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(boot_screen, COLOR_BACKGROUND, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(boot_screen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_remove_flag(boot_screen, LV_OBJ_FLAG_SCROLLABLE);

	LV_IMAGE_DECLARE(image_formlabs_logo); 
	logo_image = lv_image_create(boot_screen);
	lv_obj_set_width(logo_image, 150);
	lv_obj_set_height(logo_image, 100);
	lv_obj_set_x(logo_image, 565);
	lv_obj_set_y(logo_image, 150);
	lv_image_set_src(logo_image, &image_formlabs_logo);

	// lv_obj_add_event_cb(boot_screen, ui_screen_boot_loaded, LV_EVENT_SCREEN_LOADED, NULL);
}

lv_obj_t * ui_screen_boot(void)
{
	if (!boot_screen){
		ui_screen_boot_init();
	}
	return boot_screen;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ui_screen_boot_loaded(lv_event_t * e) {
	on_screen_loaded_cb(SCREEN_BOOT);

	/* Load home screen after boot */
    lv_screen_load_anim(ui_screen_home(), LV_SCR_LOAD_ANIM_FADE_ON, 100, 2000, false);
}

