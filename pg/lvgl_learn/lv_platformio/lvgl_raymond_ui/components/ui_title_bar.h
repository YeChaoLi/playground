/**
 * @file ui_title_bar.h
 *
 */

#ifndef UI_TITLE_BAR_H
#define UI_TITLE_BAR_H

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
 * Create the title bar inside a container
 * @param parent The parent of the title bar
 * @param title The text shown on the title bar
 * @return the title bar object (container)
 */
lv_obj_t * ui_title_bar_create(lv_obj_t * parent, const char * title);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_TITLE_BAR_H*/
