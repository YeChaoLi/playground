/**
 * @file ui_spinbox.h
 *
 */

#ifndef UI_SPINBOX_H
#define UI_SPINBOX_H

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
 * Create a spinbox inside a container
 * @param parent The parent of the spinbox object
 * @param subject The subject which will be linked to the spinbox
 * @return the spinbox object (container)
 */
lv_obj_t * ui_spinbox_create(lv_obj_t * parent, lv_subject_t * subject);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SPINBOX_H*/
