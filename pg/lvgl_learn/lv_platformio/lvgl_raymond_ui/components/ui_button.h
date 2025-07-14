/**
 * @file ui_button.h
 *
 */

#ifndef UI_BUTTON_H
#define UI_BUTTON_H

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

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a primary button (Filled blue)
 * @param parent The parent of the button object
 * @param text The text on the button
 * @return the button object
 */
lv_obj_t * ui_button_primary_create(lv_obj_t * parent, const char * text);

/**
 * Create a secondary button (Outline blue)
 * @param parent The parent of the button object
 * @param text The text on the button
 * @return the button object
 */
lv_obj_t * ui_button_secondary_create(lv_obj_t * parent, const char * text);

/**
 * Create a destructive button (Outline red)
 * @param parent The parent of the button object
 * @param text The text on the button
 * @return the button object
 */
lv_obj_t * ui_button_destructive_create(lv_obj_t * parent, const char * text);

/**
 * Create a button with icon (Outline white)
 * @param parent The parent of the button object
 * @param icon The icon on the button
 * @return the button object
 */
lv_obj_t * ui_button_icon_create(lv_obj_t * parent, const void * src);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_BUTTON_H*/
