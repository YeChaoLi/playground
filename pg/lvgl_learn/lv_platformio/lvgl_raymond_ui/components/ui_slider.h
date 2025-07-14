/**
 * @file ui_slider.h
 *
 */

#ifndef UI_SLIDER_H
#define UI_SLIDER_H

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
 * Create a slider
 * @param parent The parent of the slider object
 * @return the slider object
 */
lv_obj_t * ui_slider_create(lv_obj_t * parent);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SLIDER_H*/
