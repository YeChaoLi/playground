/**
 * @file ui_label.h
 *
 */

#ifndef UI_LABEL_H
#define UI_LABEL_H

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
 * Create a label inside a container
 * @param parent The parent of the label object
 * @param text The text on the label
 * @param width The width of the label and its container
 * @param height The height of the container
 * @return the label object (label)
 */
lv_obj_t * ui_label_create(lv_obj_t * parent, const char * text, int32_t width, int32_t height);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_LABEL_H*/
