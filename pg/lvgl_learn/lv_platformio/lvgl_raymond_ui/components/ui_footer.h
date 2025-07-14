/**
 * @file ui_footer.h
 *
 */

#ifndef UI_FOOTER_H
#define UI_FOOTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum
{
    FT_NONE = 0,
	FT_SETTINGS = (1L << 0), /* Settings Icon */
	FT_EDIT = (1L << 1),	 /* Edit Icon */
	FT_ABORT = (1L << 2),	 /* Abort Label Red */
	FT_CLOSE = (1L << 3),	 /* Close Label Blue */
	FT_DELETE = (1L << 4),	 /* Delete Label Red */

	FT_CURE = (1L << 6),   /* Cure Label Blue */
	FT_START = (1L << 7),  /* Start Label Blue Filled */
	FT_SAVE = (1L << 8),   /* Save Label Blue */
	FT_PAUSE = (1L << 9),  /* Pause Label Blue */
	FT_RESUME = (1L << 10), /* Resume Label Blue */
	FT_SKIP = (1L << 11),   /* Skip Label Blue */
	FT_DONE = (1L << 12)	   /* Done Label Blue */

} footer_button_t;

typedef struct {
    lv_obj_t *footer;
    lv_obj_t *left_cont;
    lv_obj_t *right_cont;
    lv_obj_t *ft_settings;
    lv_obj_t *ft_edit;
    lv_obj_t *ft_abort;
    lv_obj_t *ft_close;
    lv_obj_t *ft_delete;
    lv_obj_t *ft_cure;
    lv_obj_t *ft_start;
    lv_obj_t *ft_save;
    lv_obj_t *ft_pause;
    lv_obj_t *ft_resume;
    lv_obj_t *ft_skip;
    lv_obj_t *ft_done;
} ui_footer_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Creates a footer object.
 * @param parent The parent object of the footer.
 * @param btns Bitwise OR of buttons to be displayed. Additional buttons can be added/removed later using ui_footer_toggle_buttons().
 * @param callback Function to be called when a button is pressed. 
 *                 The clicked button can be retrieved as:
 *                 `footer_button_t button = (footer_button_t)(intptr_t)lv_event_get_user_data(e);`
 * @return Pointer to the created footer object.
 */
ui_footer_t *ui_footer_create(lv_obj_t *parent, footer_button_t btns, void (*callback)(lv_event_t *e));

/**
 * Updates the visible buttons in a footer object.
 * @param footer Pointer to the footer object.
 * @param btns Bitwise OR of buttons to be displayed. Only the specified buttons will be shown.
 *             Use FT_NONE to hide all buttons.
 *             Example: FT_DELETE | FT_SAVE will display only the delete and save buttons.
 */
void ui_footer_toggle_buttons(ui_footer_t *footer, footer_button_t btns);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_FOOTER_H*/
