/**
 * @file ui_footer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui_footer.h"
#include "ui_button.h"
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

static void show_button(lv_obj_t * obj, bool state);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

ui_footer_t *ui_footer_create(lv_obj_t *parent, footer_button_t btns, void (*callback)(lv_event_t *e))
{
    ui_footer_t *footer = lv_malloc(sizeof(ui_footer_t));
    if (!footer) return NULL; // Ensure memory allocation succeeded

    footer->footer = lv_obj_create(parent);
    lv_obj_remove_style_all(footer->footer);
    lv_obj_set_size(footer->footer, 1240, 84);
    lv_obj_set_pos(footer->footer, 20, 294);
    lv_obj_remove_flag(footer->footer, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    footer->left_cont = lv_obj_create(footer->footer);
    lv_obj_remove_style_all(footer->left_cont);
    lv_obj_set_size(footer->left_cont, 620, 84);
    lv_obj_set_flex_flow(footer->left_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer->left_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(footer->left_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_column(footer->left_cont, 20, LV_PART_MAIN | LV_STATE_DEFAULT);

    LV_IMAGE_DECLARE(image_icons);
	LV_IMAGE_DECLARE(image_edit_icon);

    footer->ft_settings = ui_button_icon_create(footer->left_cont, &image_icons);
    footer->ft_edit = ui_button_icon_create(footer->left_cont, &image_edit_icon);
    footer->ft_close = ui_button_secondary_create(footer->left_cont, "Close");
    footer->ft_abort = ui_button_destructive_create(footer->left_cont, "Abort");
    footer->ft_delete = ui_button_destructive_create(footer->left_cont, "Delete");

    lv_obj_add_event_cb(footer->ft_settings, callback, LV_EVENT_CLICKED, (void *)FT_SETTINGS);
    lv_obj_add_event_cb(footer->ft_edit, callback, LV_EVENT_CLICKED, (void *)FT_EDIT);
    lv_obj_add_event_cb(footer->ft_abort, callback, LV_EVENT_CLICKED, (void *)FT_ABORT);
    lv_obj_add_event_cb(footer->ft_close, callback, LV_EVENT_CLICKED, (void *)FT_CLOSE);
    lv_obj_add_event_cb(footer->ft_delete, callback, LV_EVENT_CLICKED, (void *)FT_DELETE);

	show_button(footer->ft_settings, (btns & FT_SETTINGS) == FT_SETTINGS);
	show_button(footer->ft_edit, (btns & FT_EDIT) == FT_EDIT);
	show_button(footer->ft_abort, (btns & FT_ABORT) == FT_ABORT);
	show_button(footer->ft_close, (btns & FT_CLOSE) == FT_CLOSE);
	show_button(footer->ft_delete, (btns & FT_DELETE) == FT_DELETE);

    footer->right_cont = lv_obj_create(footer->footer);
    lv_obj_remove_style_all(footer->right_cont);
    lv_obj_set_size(footer->right_cont, 620, 84);
    lv_obj_set_pos(footer->right_cont, 620, 0);
    lv_obj_set_flex_flow(footer->right_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer->right_cont, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END);
    lv_obj_remove_flag(footer->right_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_column(footer->right_cont, 20, 0);

    
    footer->ft_cure = ui_button_secondary_create(footer->right_cont, "1 min cure");
    footer->ft_start = ui_button_primary_create(footer->right_cont, "Start");
    footer->ft_skip = ui_button_secondary_create(footer->right_cont, "Skip step");
    footer->ft_save = ui_button_secondary_create(footer->right_cont, "Save");
    footer->ft_pause = ui_button_secondary_create(footer->right_cont, "Pause");
    footer->ft_resume = ui_button_secondary_create(footer->right_cont, "Resume");
    footer->ft_done = ui_button_secondary_create(footer->right_cont, "Done");

    lv_obj_add_event_cb(footer->ft_skip, callback, LV_EVENT_CLICKED, (void *)FT_SKIP);
    lv_obj_add_event_cb(footer->ft_cure, callback, LV_EVENT_CLICKED, (void *)FT_CURE);
    lv_obj_add_event_cb(footer->ft_start, callback, LV_EVENT_CLICKED, (void *)FT_START);
    lv_obj_add_event_cb(footer->ft_save, callback, LV_EVENT_CLICKED, (void *)FT_SAVE);
    lv_obj_add_event_cb(footer->ft_pause, callback, LV_EVENT_CLICKED, (void *)FT_PAUSE);
    lv_obj_add_event_cb(footer->ft_resume, callback, LV_EVENT_CLICKED, (void *)FT_RESUME);
    lv_obj_add_event_cb(footer->ft_done, callback, LV_EVENT_CLICKED, (void *)FT_DONE);


	show_button(footer->ft_skip, (btns & FT_SKIP) == FT_SKIP);
	show_button(footer->ft_cure, (btns & FT_CURE) == FT_CURE);
	show_button(footer->ft_start, (btns & FT_START) == FT_START);
	show_button(footer->ft_save, (btns & FT_SAVE) == FT_SAVE);
	show_button(footer->ft_pause, (btns & FT_PAUSE) == FT_PAUSE);
	show_button(footer->ft_resume, (btns & FT_RESUME) == FT_RESUME);
	show_button(footer->ft_done, (btns & FT_DONE) == FT_DONE);

    return footer;
}

void ui_footer_toggle_buttons(ui_footer_t *footer, footer_button_t btns)
{
    if (!footer) return;

    show_button(footer->ft_settings, (btns & FT_SETTINGS) == FT_SETTINGS);
    show_button(footer->ft_edit, (btns & FT_EDIT) == FT_EDIT);
    show_button(footer->ft_abort, (btns & FT_ABORT) == FT_ABORT);
    show_button(footer->ft_close, (btns & FT_CLOSE) == FT_CLOSE);
    show_button(footer->ft_delete, (btns & FT_DELETE) == FT_DELETE);
    show_button(footer->ft_skip, (btns & FT_SKIP) == FT_SKIP);
    show_button(footer->ft_cure, (btns & FT_CURE) == FT_CURE);
    show_button(footer->ft_start, (btns & FT_START) == FT_START);
    show_button(footer->ft_save, (btns & FT_SAVE) == FT_SAVE);
    show_button(footer->ft_pause, (btns & FT_PAUSE) == FT_PAUSE);
    show_button(footer->ft_resume, (btns & FT_RESUME) == FT_RESUME);
    show_button(footer->ft_done, (btns & FT_DONE) == FT_DONE);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void show_button(lv_obj_t * obj, bool state)
{

    if(state) {
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }

}